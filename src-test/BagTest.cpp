/*
 * BagTest.cpp
 *
 *  Created on: 07.10.2011
 *      Author: wbam
 */

#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include "CsvMapReader.hpp"
#include <boost/program_options.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <iostream>

using namespace cpprob;
using namespace std;

extern boost::program_options::variables_map options_map;

class PutOutParameters : public boost::static_visitor<>
{

public:

  PutOutParameters(ostream& os)
      : os_(os)
  {
  }

  void
  operator()(const ConditionalDirichletNode& node)
  {
    if (!node.is_evidence())
      os_ << node.value() << "\n";
  }

  void
  operator()(const DirichletNode& node)
  {
    if (!node.is_evidence())
      os_ << node.value() << "\n";
  }

  template<class N>
    void
    operator()(const N&)
    {
    }

private:

  ostream& os_;

};

BayesianNetwork
gen_bag_net(float alpha, bool fully_observed, size_t lines_of_evidence =
    std::numeric_limits<std::size_t>::max())
{
  BayesianNetwork bn;

  // Set up the parameter vertices
  map<string, ConditionalDirichletNode*> params_table;
  RandomBoolean bag("Bag", true);
  RandomProbabilities bag_params(bag);
  DirichletNode& bag_params_node = bn.add_dirichlet(bag_params, alpha);
  RandomConditionalProbabilities flavor_params(RandomBoolean("Flavor", true),
      bag);
  params_table.insert(
      make_pair("Flavor", &bn.add_conditional_dirichlet(flavor_params, alpha)));
  RandomConditionalProbabilities wrapper_params(RandomBoolean("Wrapper", true),
      bag);
  params_table.insert(
      make_pair("Wrapper",
          &bn.add_conditional_dirichlet(wrapper_params, alpha)));
  RandomConditionalProbabilities hole_params(RandomBoolean("Hole", true), bag);
  params_table.insert(
      make_pair("Hole", &bn.add_conditional_dirichlet(hole_params, alpha)));

  // Read the data from the file
  CsvMapReader reader(options_map["data-file"].as<string>());
  CsvMapReader::Records full_data = reader.read_rows();

  // Fill the data in the net
  size_t line = 0;
  CsvMapReader::Records::iterator r = full_data.begin();
  for (; r != full_data.end() && line != lines_of_evidence; ++r, ++line)
  {
    RandomBoolean bag_evidence("Bag", r->operator[]("Bag"));
    CategoricalNode& bag_evidence_node = bn.add_categorical(bag_evidence,
        bag_params_node);
    if (fully_observed)
      bag_evidence_node.is_evidence(true);

    for (CsvMapReader::NamedAttributes::iterator a = r->begin(); a != r->end();
        ++a)
    {
      if (a->first != "Bag")
      {
        RandomBoolean var(a->first, a->second);
        ConditionalCategoricalNode& node = bn.add_conditional_categorical(var,
          { &bag_evidence_node }, *params_table[a->first]);
        node.is_evidence(true);
      }
    }
  }

  return bn;
}

BOOST_AUTO_TEST_CASE( bag_test )
{
  BOOST_REQUIRE_MESSAGE(options_map.count("data-file") == 1,
      "Please provide test data with the option --data-file.");

  cout << "Generate the full hybrid bag network\n";
  double duration;
  boost::timer t;
  BayesianNetwork bn_map_full = gen_bag_net(5.0, true);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << endl;

  if (options_map["with-debug-output"].as<bool>())
    cout << bn_map_full << endl;

  cout << "Learn MAP on full data\n";
  t.restart();
  bn_map_full.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  for_each(bn_map_full.begin(), bn_map_full.end(),
      make_apply_visitor_delayed(PutOutParameters(cout)));
  cout << endl;

  cout << "Learn ML on full data\n";
  BayesianNetwork bn_ml_full = gen_bag_net(0.0, true);
  t.restart();
  bn_ml_full.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  for_each(bn_ml_full.begin(), bn_ml_full.end(),
      make_apply_visitor_delayed(PutOutParameters(cout)));
  cout << endl;

  cout << "Learn MAP on partial data\n";
  BayesianNetwork bn_map_partial = gen_bag_net(5.0, true, 5);
  t.restart();
  bn_map_partial.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  for_each(bn_map_partial.begin(), bn_map_partial.end(),
      make_apply_visitor_delayed(PutOutParameters(cout)));
  cout << endl;

  cout << "Learn ML on partial data\n";
  BayesianNetwork bn_ml_partial = gen_bag_net(0.0, true, 5);
  t.restart();
  bn_ml_partial.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  for_each(bn_ml_partial.begin(), bn_ml_partial.end(),
      make_apply_visitor_delayed(PutOutParameters(cout)));
  cout << endl;

  cout << "Extend the network for prediction\n";

  DirichletNode& bag_params_v = bn_map_full.at<DirichletNode>(
      "ProbabilitiesBag");
  RandomBoolean bag("Bag", true);
  CategoricalNode& bag_v = bn_map_full.add_categorical(bag, bag_params_v);

  ConditionalDirichletNode& hole_params_v = bn_map_full.at<
      ConditionalDirichletNode>("ProbabilitiesHoleBag");
  RandomBoolean hole("Hole", true);
  ConditionalCategoricalNode& hole_v = bn_map_full.add_conditional_categorical(
      hole,
        { &bag_v }, hole_params_v);
  hole_v.is_evidence(true);

  ConditionalDirichletNode& wrapper_params_v = bn_map_full.at<
      ConditionalDirichletNode>("ProbabilitiesWrapperBag");
  RandomBoolean wrapper("Wrapper", true);
  ConditionalCategoricalNode& wrapper_v =
      bn_map_full.add_conditional_categorical(wrapper,
        { &bag_v }, wrapper_params_v);
  wrapper_v.is_evidence(true);

  ConditionalDirichletNode& flavor_params_v = bn_map_full.at<
      ConditionalDirichletNode>("ProbabilitiesFlavorBag");
  RandomBoolean flavor("Flavor", true);
  ConditionalCategoricalNode& flavor_v =
      bn_map_full.add_conditional_categorical(flavor,
        { &bag_v }, flavor_params_v);

  if (options_map["with-debug-output"].as<bool>())
    cout << bn_map_full << endl;

  unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
      unsigned int>();
  unsigned int collect_iterations = options_map["collect-iterations"].as<
      unsigned int>();
  random_number_engine.seed(); // Reset in a well-defined state.
  cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  CategoricalDistribution prediction = bn_map_full.sample(flavor_v,
      burn_in_iterations, collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Predictive distribution with sampling:\n";
  cout << prediction;
  /*
   * The counts in bag.csv are
   *   C(F=0 | H=1, W=1):  79 (grep -e "false,true,true" bag.csv | wc)
   *   C(F=1 | H=1, W=1): 272 (grep -e "true,true,true" bag.csv | wc)
   *
   * With prior parameters (5, 5) the counts are 84/277. This results in
   * the probabilities 0.2327/0.7673.
   */
  cout << "Correct values (bag.csv):\n";
  cout << "      (Flavor:0,0.2327)  (Flavor:1,0.7673)\n" << endl;

  random_number_engine.seed(); // Reset in a well-defined state.
  cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  prediction = bn_map_full.sample(bag_v, burn_in_iterations,
      collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Predictive distribution with sampling:\n";
  cout << prediction;
  /*
   * The counts in bag.csv are
   *   C(B=0 | H=1, W=1): 302 (grep -e "false,.*,true,true" bag.csv | wc)
   *   C(B=1 | H=1, W=1):  50 (grep -e "true,.*,true,true" bag.csv | wc)
   *
   * With prior parameters (5, 5) the counts are 307/55. This results in
   * the probabilities 0.8481/0.1519.
   */
  cout << "Correct values (bag.csv):\n";
  cout << "      (Bag:0,0.8481)  (Bag:1,0.1519)" << endl;
}

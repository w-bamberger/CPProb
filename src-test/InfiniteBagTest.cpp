/*
 * InfiniteBagTest.cpp
 *
 *  Created on: 07.10.2011
 *      Author: wbam
 */

#include "CsvMapReader.hpp"
#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include <boost/program_options.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>

using namespace cpprob;
using namespace std;

extern boost::program_options::variables_map options_map;

BayesianNetwork
gen_infinite_bag_net(float alpha, size_t lines_of_evidence =
    std::numeric_limits<std::size_t>::max())
{
  const DiscreteRandomReferences no_condition;
  BayesianNetwork bn;

  // Set up the parameter vertices
  map<string, ConditionalDirichletNode*> params_table;
  RandomBoolean bag("Bag", true);
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
  ConstantNode<DirichletProcessParameters>& bag_params_node =
      bn.add_dirichlet_process_parameters(bag.name(), 2.0,
          boost::adaptors::values(params_table));

  // Read the data from the file
  CsvMapReader reader(options_map["data-file"].as<string>());
  CsvMapReader::Records full_data = reader.read_rows();

  // Fill the data in the net
  size_t line = 0;
  CsvMapReader::Records::iterator r = full_data.begin();
  for (; r != full_data.end() && line != lines_of_evidence; ++r, ++line)
  {
    RandomBoolean bag_evidence("Bag", r->operator[]("Bag"));
    DirichletProcessNode& bag_evidence_node = bn.add_dirichlet_process(
        bag_params_node);

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

template<class K, class V>
ostream&
put_out_map(ostream& os, const cont::map<K, V>& m)
{
  for (auto it = m.begin(); it != m.end(); ++it)
    os << it->first << ", " << it->second << "\n";
  return os;
}

BOOST_AUTO_TEST_CASE(InfiniteBagTest)
{
  BOOST_REQUIRE_MESSAGE(options_map.count("data-file") == 1,
      "Please provide test data with the option --data-file.");

  cout << "Generate the bag network with infinite mixtures\n";
  double duration;
  boost::timer t;
  BayesianNetwork bn = gen_infinite_bag_net(5.0);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << endl;

  if (options_map["with-debug-output"].as<bool>())
    cout << bn << endl;

  ConstantNode<DirichletProcessParameters>& bag_params = bn.find<
      ConstantNode<DirichletProcessParameters> >("Bag");
  DirichletProcessNode& bag_v = bn.add_dirichlet_process(bag_params);

  ConditionalDirichletNode& hole_params_v = bn.find<ConditionalDirichletNode>(
      "ProbabilitiesHoleBag");
  RandomBoolean hole("Hole", true);
  ConditionalCategoricalNode& hole_v = bn.add_conditional_categorical(hole,
    { &bag_v }, hole_params_v);
  hole_v.is_evidence(true);

  ConditionalDirichletNode& wrapper_params_v =
      bn.find<ConditionalDirichletNode>("ProbabilitiesWrapperBag");
  RandomBoolean wrapper("Wrapper", true);
  ConditionalCategoricalNode& wrapper_v = bn.add_conditional_categorical(
      wrapper,
        { &bag_v }, wrapper_params_v);
  wrapper_v.is_evidence(true);

  ConditionalDirichletNode& flavor_params_v = bn.find<ConditionalDirichletNode>(
      "ProbabilitiesFlavorBag");
  RandomBoolean flavor("Flavor", true);
  ConditionalCategoricalNode& flavor_v = bn.add_conditional_categorical(flavor,
    { &bag_v }, flavor_params_v);

  unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
      unsigned int>();
  unsigned int collect_iterations = options_map["collect-iterations"].as<
      unsigned int>();
  random_number_engine.seed(); // Reset in a well-defined state.
  cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  CategoricalDistribution prediction = bn.sample(flavor_v, burn_in_iterations,
      collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Predictive distribution with sampling:\n";
  cout << prediction << endl;
  /*
   * The counts in latent-bag.csv are
   *   C(F=0 | H=1, W=1): 52 (grep -e "false,true,true" bag.csv | wc)
   *   C(F=1 | H=1, W=1): 20 (grep -e "true,true,true" bag.csv | wc)
   *
   * With prior parameters (5, 5) the counts are 57/25. This results in
   * the probabilities 0.6951/0.3049.
   */
  cout << "Correct values (latent-bag.csv):\n";
  cout << "      (Flavor:0,0.6951)  (Flavor:1,0.3049)\n" << endl;
  put_out_map(cout, bag_params.value().component_counters());
}


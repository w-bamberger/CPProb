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
#include <boost/test/floating_point_comparison.hpp>
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
#ifndef WITHOUT_INITIALIZER_LIST
        ConditionalCategoricalNode& node = bn.add_conditional_categorical(var,
          { &bag_evidence_node }, *params_table[a->first]);
#else
        cont::RefVector<DiscreteNode> parents(1, bag_evidence_node);
        ConditionalCategoricalNode& node = bn.add_conditional_categorical(var,
            parents, *params_table[a->first]);
#endif
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
#ifndef WITHOUT_INITIALIZER_LIST
  ConditionalCategoricalNode& hole_v = bn_map_full.add_conditional_categorical(
      hole, { &bag_v }, hole_params_v);
#else
  cont::RefVector<DiscreteNode> parents(1, bag_v);
  ConditionalCategoricalNode& hole_v = bn_map_full.add_conditional_categorical(
      hole, parents, hole_params_v);
#endif
  hole_v.is_evidence(true);

  ConditionalDirichletNode& wrapper_params_v = bn_map_full.at<
      ConditionalDirichletNode>("ProbabilitiesWrapperBag");
  RandomBoolean wrapper("Wrapper", true);
#ifndef WITHOUT_INITIALIZER_LIST
  ConditionalCategoricalNode& wrapper_v =
      bn_map_full.add_conditional_categorical(wrapper,
        { &bag_v }, wrapper_params_v);
#else
  ConditionalCategoricalNode& wrapper_v =
      bn_map_full.add_conditional_categorical(wrapper,
        parents, wrapper_params_v);
#endif
  wrapper_v.is_evidence(true);

  ConditionalDirichletNode& flavor_params_v = bn_map_full.at<
      ConditionalDirichletNode>("ProbabilitiesFlavorBag");
  RandomBoolean flavor("Flavor", true);
#ifndef WITHOUT_INITIALIZER_LIST
  ConditionalCategoricalNode& flavor_v =
      bn_map_full.add_conditional_categorical(flavor,
        { &bag_v }, flavor_params_v);
#else
  ConditionalCategoricalNode& flavor_v =
      bn_map_full.add_conditional_categorical(flavor,
        parents, flavor_params_v);
#endif

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
  CategoricalDistribution prediction = bn_map_full.sample(bag_v,
      burn_in_iterations, collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
  {
    cout << "Duration: " << duration << "\n";
    cout << "Predictive distribution with sampling:\n";
    cout << prediction;
    /*
     * The counts in bag.csv are
     *
     *   NBf = C(B=f):         496 (grep "^false" bag.csv | wc)
     *   NBt = C(B=t):         504 (grep "^true" bag.csv | wc)
     *   NBfWf = C(W=f | B=f): 119 (grep "^false,.*,false," bag.csv | wc)
     *   NBfWt = C(W=t | B=f): 377 (grep "^false,.*,true," bag.csv | wc)
     *   NBtWf = C(W=f | B=t): 336 (grep "^true,.*,false," bag.csv | wc)
     *   NBtWt = C(W=t | B=t): 168 (grep "^true,.*,true," bag.csv | wc)
     *   NBfHf = C(H=f | B=f):  95 (grep "^false,.*,false$" bag.csv | wc)
     *   NBfHt = C(H=t | B=f): 401 (grep "^false,.*,true$" bag.csv | wc)
     *   NBtHf = C(H=f | B=t): 355 (grep "^true,.*,false$" bag.csv | wc)
     *   NBtHt = C(H=t | B=t): 149 (grep "^true,.*,true$" bag.csv | wc)
     *
     * The hyper parameters (a, a) = (5, 5). The aim is to find
     * the probabilities
     *
     *   P(B=s | W=t, H=t, B_, W_, H_) \propto
     *     (a + NBs) L(W=t | B=s) L(H=t | B=s),
     *
     * where s is either t or f. The first term is the prior probability
     * of B=s for the collapsed Dirichlet-categorical distribution.
     *
     * The likelihood L(W=t | B=s) corresponds to the probability
     * P(W=t | B=s) in the component B=s of the mixture model for W. These
     * components again are collapsed Dirichlet-categorical distributions.
     * The same holds for H=t. Thus their probabilities are
     *
     *   P(W=t | B=f) = (a + NBfWt) / (a + NBfWt + a + NBfWf) =
     *     (5 + 377) / (5 + 377 + 5 + 119) = 382 / 506 = 0.7549
     *   P(W=t | B=t) = (a + NBtWt) / (a + NBtWt + a + NBtWf) =
     *     (5 + 168) / (5 + 168 + 5 + 336) = 173 / 514 = 0.3366
     *   P(H=t | B=f) = (a + NBfHt) / (a + NBfHt + a + NBfHf) =
     *     (5 + 401) / (5 + 401 + 5 + 95) = 406 / 506 = 0.8024
     *   P(H=t | B=t) = (a + NBtHt) / (a + NBtHt + a + NBtHf) =
     *     (5 + 149) / (5 + 149 + 5 + 355) = 154 / 514 = 0.2996
     *
     * Filling these likelihoods in the equation at the beginning results in
     * the unnormalised probabilities
     *
     *   P(B=f | W=t, H=t, B_, W_, H_) \propto
     *     (5 + 496) * 0.7549 * 0.8024 = 303.5
     *   P(B=t | W=t, H=t, B_, W_, H_) \propto
     *     (5 + 504) * 0.3366 * 0.2996 = 51.33
     *
     * After normalisation, the probabilities are
     *
     *   P(B=f | W=t, H=t, B_, W_, H_) = 0.85533356
     *   P(B=t | W=t, H=t, B_, W_, H_) = 0.14466644
     *
     */
    cout << "Correct values (bag.csv):\n";
    cout << "      (Bag:0,0.8553)  (Bag:1,0.1447)\n" << endl;
  } //
  BOOST_CHECK_SMALL(prediction.begin()->second - 0.85533356f, 0.01f);

  random_number_engine.seed(); // Reset in a well-defined state.
  float max_error = 0.01f;
  unsigned int iterations = 0;
  cout << "Sample with automatic convergence at a precision of " << max_error
      << "\n";
  t.restart();
  prediction = bn_map_full.sample(bag_v, 0.01f, &iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
  {
    cout << "Duration: " << duration << "\n";
    cout << iterations << " iterations\n";
    cout << "Predictive distribution with sampling:\n";
    cout << prediction << endl;
  } //
  BOOST_CHECK_SMALL(prediction.begin()->second - 0.85533356f, 0.01f);

  random_number_engine.seed(); // Reset in a well-defined state.
  cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  prediction = bn_map_full.sample(flavor_v,
      burn_in_iterations, collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
  {
    cout << "Duration: " << duration << "\n";
    cout << "Predictive distribution with sampling:\n";
    cout << prediction;
    /*
    * The counts in bag.csv are
    *
    *   NBfFf = C(F=f | B=f):  89 (grep "^false,false" bag.csv | wc)
    *   NBfFt = C(F=t | B=f): 407 (grep "^false,true" bag.csv | wc)
    *   NBtFf = C(F=f | B=t): 351 (grep "^true,false" bag.csv | wc)
    *   NBtFt = C(F=t | B=t): 153 (grep "^true,true" bag.csv | wc)
    *
    * The hyper parameters are (a, a) = (5, 5).
    * This leads to the unnormalised probabilities
    *
    * P(F=f | H=T, W=T, F_, B_, W_, H_) \propto
    *   (a + NBfFf) P(B=f | H=T, W=T, B_, , W_, H_) +
    *     (a + NBtFf) P(B=t | H=T, W=T, B_, W_, H_) =
    *   131.9
    *
    * P(F=t | H=T, W=T, F_, B_, W_, H_) \propto
    *   (a + NBfFt) P(B=f | H=T, W=T, B_, W_, H_) +
    *     (a + NBtFt) P(B=t | H=T, W=T, B_, W_, H_) =
    *  375.3
    *
    * Normalising them results in
    *
    * P(F=f | H=T, W=T, F_, B_, W_, H_) = 0.26008222
    * P(F=t | H=T, W=T, F_, B_, W_, H_) = 0.73991778
    */
    cout << "Correct values (bag.csv):\n";
    cout << "      (Flavor:0,0.2601)  (Flavor:1,0.7399)\n" << endl;
  } //
  BOOST_CHECK_SMALL(prediction.begin()->second - 0.26008222f, 0.01f);

  random_number_engine.seed(); // Reset in a well-defined state.
  max_error = 0.01f;
  cout << "Sample with automatic convergence at a precision of " << max_error
      << "\n";
  t.restart();
  prediction = bn_map_full.sample(flavor_v, max_error, &iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
  {
    cout << "Duration: " << duration << "\n";
    cout << iterations << " iterations\n";
    cout << "Predictive distribution with sampling:\n";
    cout << prediction << endl;
  }
  BOOST_CHECK_SMALL(prediction.begin()->second - 0.26008222f, 0.01f);
}

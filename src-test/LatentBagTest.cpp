/*
 * LatentBagTest.cpp
 *
 *  Created on: 07.10.2011
 *      Author: wbam
 */

#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include <boost/program_options.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <iostream>

using namespace cpprob;
using namespace std;

BayesianNetwork
gen_bag_net(float alpha, bool fully_observed, size_t lines_of_evidence =
    std::numeric_limits<std::size_t>::max());
extern boost::program_options::variables_map options_map;

BOOST_AUTO_TEST_CASE( latent_bag_test )
{
  BOOST_REQUIRE_MESSAGE(options_map.count("data-file") == 1,
      "Please provide test data with the option --data-file.");

  cout << "Generate the bag network with all data but no bag observations\n";
  double duration;
  boost::timer t;
  BayesianNetwork bn = gen_bag_net(5.0, false);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << endl;

  if (options_map["with-debug-output"].as<bool>())
    cout << bn << endl;

  DirichletNode& bag_params_v = bn.at<DirichletNode>("ProbabilitiesBag");
  RandomBoolean bag("Bag", true);
  CategoricalNode& bag_v = bn.add_categorical(bag, bag_params_v);

  ConditionalDirichletNode& hole_params_v = bn.at<ConditionalDirichletNode>(
      "ProbabilitiesHoleBag");
  RandomBoolean hole("Hole", true);
  ConditionalCategoricalNode& hole_v = bn.add_conditional_categorical(hole,
    { &bag_v }, hole_params_v);
  hole_v.is_evidence(true);

  ConditionalDirichletNode& wrapper_params_v =
      bn.at<ConditionalDirichletNode>("ProbabilitiesWrapperBag");
  RandomBoolean wrapper("Wrapper", true);
  ConditionalCategoricalNode& wrapper_v = bn.add_conditional_categorical(
      wrapper,
        { &bag_v }, wrapper_params_v);
  wrapper_v.is_evidence(true);

  ConditionalDirichletNode& flavor_params_v = bn.at<ConditionalDirichletNode>(
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

  random_number_engine.seed(); // Reset in a well-defined state.
  cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  prediction = bn.sample(bag_v, burn_in_iterations, collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Predictive distribution with sampling:\n";
  cout << prediction << endl;
  /*
   * The counts in latent-bag.csv are
   *   C(B=0 | H=1, W=1): 48 (grep -e "false,.*,true,true" bag.csv | wc)
   *   C(B=1 | H=1, W=1): 24 (grep -e "true,.*,true,true" bag.csv | wc)
   *
   * With prior parameters (5, 5) the counts are 53/29. This results in
   * the probabilities 0.6463/0.3537.
   */
  cout << "Correct values (latent-bag.csv):\n";
  cout << "      (Bag:0,0.6463)  (Bag:1,0.3537)" << endl;

//  for (int i = 1; i <= 10; ++i)
//  {
//    collect_iterations = i * 5000;
//    cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
//        << collect_iterations << " collect iterations.\n";
//    t.restart();
//    CategoricalDistribution prediction = bn.sample(flavor_v, burn_in_iterations,
//        collect_iterations);
//    duration = t.elapsed();
//    cout << "Duration: " << duration << "\n";
//    cout << "Predictive distribution with sampling:" << prediction << endl;
//  }

//  for (int i = 1; i < 5; ++i)
//  {
//    collect_iterations = 5000;
//    cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
//        << collect_iterations << " collect iterations.\n";
//    t.restart();
//    CategoricalDistribution prediction = bn.sample(flavor_v, burn_in_iterations,
//        collect_iterations);
//    duration = t.elapsed();
//    cout << "Duration: " << duration << "\n";
//    cout << "Predictive distribution with sampling:" << prediction << endl;
//  }
}

/*
 * AlarmTest.cpp
 *
 *  Created on: 07.10.2011
 *      Author: wbam
 */

#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/DiscreteJointRandomVariable.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include <boost/program_options.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <iostream>

using namespace cpprob;
using namespace std;

extern boost::program_options::variables_map options_map;

BayesianNetwork
gen_alarm_net()
{
  typedef BayesianNetwork::iterator vertex_iterator;
  BayesianNetwork bn;
  const DiscreteJointRandomVariable no_parents;

  RandomBoolean burglary("Burglary", true);
  CategoricalNode& burglary_node = bn.add_categorical(burglary);
  RandomProbabilities& burglary_probs = burglary_node.probabilities();
  burglary_probs.set(burglary, 0.001f);
  burglary_probs.set(burglary.observation(false), 0.999f);

  RandomBoolean earthquake("Earthquake", true);
  CategoricalNode& earthquake_node = bn.add_categorical(earthquake);
  RandomProbabilities& earthquake_probs = earthquake_node.probabilities();
  earthquake_probs.set(earthquake, 0.002f);
  earthquake_probs.set(earthquake.observation(false), 0.998f);

  RandomBoolean alarm("Alarm", true);

  RandomConditionalProbabilities alarm_params(alarm,
      DiscreteJointRandomVariable(burglary, earthquake));
  burglary.observation(true);
  earthquake.observation(true);
  alarm_params.set(alarm.observation(true),
      DiscreteJointRandomVariable(burglary, earthquake), 0.95f);
  alarm_params.set(alarm.observation(false),
      DiscreteJointRandomVariable(burglary, earthquake), 0.05f);
  earthquake.observation(false);
  alarm_params.set(alarm.observation(true),
      DiscreteJointRandomVariable(burglary, earthquake), 0.94f);
  alarm_params.set(alarm.observation(false),
      DiscreteJointRandomVariable(burglary, earthquake), 0.06f);
  burglary.observation(false);
  earthquake.observation(true);
  alarm_params.set(alarm.observation(true),
      DiscreteJointRandomVariable(burglary, earthquake), 0.29f);
  alarm_params.set(alarm.observation(false),
      DiscreteJointRandomVariable(burglary, earthquake), 0.71f);
  earthquake.observation(false);
  alarm_params.set(alarm.observation(true),
      DiscreteJointRandomVariable(burglary, earthquake), 0.001f);
  alarm_params.set(alarm.observation(false),
      DiscreteJointRandomVariable(burglary, earthquake), 0.999f);
  ConstantRandomConditionalProbabilitiesNode& alarm_params_node =
      bn.add_constant(alarm_params);

  ConditionalCategoricalNode& alarm_node = bn.add_conditional_categorical(
      alarm,
        { &burglary_node, &earthquake_node }, alarm_params_node);

  RandomBoolean john_calls("JohnCalls", true);
  ConditionalCategoricalNode& john_calls_node =
      bn.add_conditional_categorical(john_calls, {&alarm_node});
  john_calls_node.is_evidence(true);
  RandomConditionalProbabilities& john_calls_probs =
      john_calls_node.probabilities();
  alarm.observation(true);
  john_calls_probs.set(john_calls.observation(true), alarm, 0.9f);
  john_calls_probs.set(john_calls.observation(false), alarm, 0.1f);
  alarm.observation(false);
  john_calls_probs.set(john_calls.observation(true), alarm, 0.05f);
  john_calls_probs.set(john_calls.observation(false), alarm, 0.95f);

  RandomBoolean mary_calls("MaryCalls", true);
  ConditionalCategoricalNode& mary_calls_node =
      bn.add_conditional_categorical(mary_calls, {&alarm_node});
  mary_calls_node.is_evidence(true);
  RandomConditionalProbabilities& mary_calls_probs =
      mary_calls_node.probabilities();
  alarm.observation(true);
  mary_calls_probs.set(mary_calls.observation(true), alarm, 0.7f);
  mary_calls_probs.set(mary_calls.observation(false), alarm, 0.3f);
  alarm.observation(false);
  mary_calls_probs.set(mary_calls.observation(true), alarm, 0.01f);
  mary_calls_probs.set(mary_calls.observation(false), alarm, 0.99f);

  return bn;
}

BOOST_AUTO_TEST_CASE( alarm_test )
{
  double duration;
  cout << "Generate the hybrid alarm network\n";
  boost::timer t;
  BayesianNetwork bn = gen_alarm_net();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n" << endl;
  BOOST_CHECK_MESSAGE(bn.size() == 10, "Network size: " << bn.size());
  if (options_map["with-debug-output"].as<bool>())
    cout << bn << endl;

  CategoricalNode& burglary_node = bn.at<CategoricalNode>("Burglary");

  cout << "Enumerate\n";
  t.restart();
  CategoricalDistribution burglary_distribution = bn.enumerate(burglary_node);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Burglary distribution with enumeration:\n";
  cout << burglary_distribution;
  cout << endl;

  unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
      unsigned int>();
  unsigned int collect_iterations = options_map["collect-iterations"].as<
      unsigned int>();
  cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  burglary_distribution = bn.sample(burglary_node, burn_in_iterations,
      collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Burglary distribution with Gibbs sampling:\n";
  cout << burglary_distribution;
  cout << endl;
}

/*
 * main.cpp
 *
 *  Created on: 13.05.2011
 *      Author: wbam
 */

#include "network_generator.hpp"
#include "../src-lib/boolean_random_variable.hpp"
#include "../src-lib/discrete_joint_random_variable.hpp"
#include "../src-lib/io_utils.hpp"
#include "../src-lib/utils.hpp"
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/timer.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>

extern "C" void
__libc_freeres(void);

using namespace boost;
using namespace boost::assign;
using namespace std;
using namespace cpprob;

enum TestCase
{
  alarm_test, bag_test, latent_bag_test
};

std::istream&
operator>>(std::istream& is, TestCase& tc)
{
  std::string value;
  is >> value;

  if (value == "alarm-test")
    tc = alarm_test;
  else if (value == "bag-test")
    tc = bag_test;
  else if (value == "latent-bag-test")
    tc = latent_bag_test;
  else
    cpprob_throw_runtime_error(
        "Wrong argument for test case: " << value << ".");

  return is;
}

program_options::variables_map options_map;

ostream&
put_out_probability_variables(ostream& os, BayesianNetwork& bn);

void
test_alarm_net();

void
test_bag_net();

void
test_latent_bag_net();

int
main(int argc, char **argv)
{
  atexit(__libc_freeres);

  program_options::options_description options_desc("Allowed options");
  options_desc.add_options() //
  ("help", "show this help message") //
  ("burn-in-iterations",
      program_options::value<unsigned int>()->default_value(200),
      "number of iterations samples of which are not saved") //
  ("collect-iterations",
      program_options::value<unsigned int>()->default_value(800),
      "number of iterations during which the samples are counted") //
  ("data-file", program_options::value<string>(),
      "CSV file with the data used for learning and inference") //
  ("test-case",
      program_options::value<TestCase>()->default_value(latent_bag_test),
      "Choose what to test") //
  ("test-mode",
      program_options::value<bool>()->default_value(false)->zero_tokens(),
      "reduce the output so it fits for automated testing with texttest") //
  ("with-debug-output",
      program_options::value<bool>()->default_value(false)->zero_tokens(),
      "print detailed information about the result of every step");
  program_options::store(
      program_options::parse_command_line(argc, argv, options_desc),
      options_map);
  program_options::notify(options_map);
  if (options_map.count("help"))
  {
    cout << options_desc << "\n";
    return 0;
  }

  switch (options_map["test-case"].as<TestCase>())
  {
  case alarm_test:
    test_alarm_net();
    break;

  case bag_test:
    if (options_map.count("data-file") != 1)
    {
      cerr << "Please provide test data with the option --data-file." << endl;
      return 1;
    }
    test_bag_net();
    break;

  case latent_bag_test:
    if (options_map.count("data-file") != 1)
    {
      cerr << "Please provide test data with the option --data-file." << endl;
      return 1;
    }
    test_latent_bag_net();
    break;

  default:
    cerr << "Invalid test found: " << options_map["test-case"].as<TestCase>()
        << "." << endl;
    return 1;

  }

  return 0;
}

ostream&
put_out_probability_variables(ostream& os, BayesianNetwork& bn)
{
  for (BayesianNetwork::const_iterator v = bn.begin(); v != bn.end(); ++v)
  {
    if (!v->value_is_evidence())
    {
      if (const RandomProbabilities* ps = dynamic_cast<const RandomProbabilities*>(&v->random_variable()))
      {
        os << *ps << "\n";
      }
      else if (const RandomConditionalProbabilities* cps = dynamic_cast<const RandomConditionalProbabilities*>(&v->random_variable()))
      {
        os << *cps << "\n";
      }
    }
  }
  return os;
}

void
test_alarm_net()
{
  double duration;
  cout << "Generate the hybrid alarm network\n";
  boost::timer t;
  BayesianNetwork bn = NetworkGenerator::gen_alarm_net();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n" << endl;
  if (options_map["with-debug-output"].as<bool>())
    cout << bn << endl;

  BayesianNetwork::iterator search_it = find_if(bn.begin(), bn.end(),
      BayesianNetwork::CompareVertexName("Burglary"));

  cout << "Enumerate\n";
  t.restart();
  CategoricalDistribution burglary_distribution = bn.enumerate(search_it);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Burglary distribution with enumeration:\n";
  cout << burglary_distribution;
  cout << endl;

  search_it->value_is_evidence(false);

  unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
      unsigned int>();
  unsigned int collect_iterations = options_map["collect-iterations"].as<
      unsigned int>();
  cout << "Sample with " << burn_in_iterations << " burn in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  burglary_distribution = bn.sample(search_it, burn_in_iterations,
      collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Burglary distribution with Gibbs sampling:\n";
  cout << burglary_distribution;
  cout << endl;
}

void
test_bag_net()
{
  cout << "Generate the full hybrid bag network\n";
  double duration;
  boost::timer t;
  BayesianNetwork bn_map_full = NetworkGenerator::gen_bag_net(5.0, true);
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
  put_out_probability_variables(cout, bn_map_full);
  cout << endl;

  cout << "Learn ML on full data\n";
  BayesianNetwork bn_ml_full = NetworkGenerator::gen_bag_net(0.0, true);
  t.restart();
  bn_ml_full.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  put_out_probability_variables(cout, bn_ml_full);
  cout << endl;

  cout << "Learn MAP on partial data\n";
  BayesianNetwork bn_map_partial = NetworkGenerator::gen_bag_net(5.0, true, 5);
  t.restart();
  bn_map_partial.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  put_out_probability_variables(cout, bn_map_partial);
  cout << endl;

  cout << "Learn ML on partial data\n";
  BayesianNetwork bn_ml_partial = NetworkGenerator::gen_bag_net(0.0, true, 5);
  t.restart();
  bn_ml_partial.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  put_out_probability_variables(cout, bn_ml_partial);
  cout << endl;

  cout << "Extend the network for sampling\n";

  BayesianNetwork::iterator bag_params_v = find_if(bn_map_full.begin(),
      bn_map_full.end(),
      BayesianNetwork::CompareVertexName("ProbabilitiesBag"));

  BooleanRandomVariable bag("Bag", true);
  BayesianNetwork::iterator bag_v = bn_map_full.add_categorical(bag,
      bag_params_v);
  bag_v->value_is_evidence(true);

  BayesianNetwork::iterator hole_params_v = find_if(bn_map_full.begin(),
      bn_map_full.end(),
      BayesianNetwork::CompareVertexName("ProbabilitiesHoleBag"));
  BooleanRandomVariable hole("Hole", true);
  BayesianNetwork::iterator hole_v = bn_map_full.add_conditional_categorical(
      hole, list_of(bag_v), hole_params_v);

  BayesianNetwork::iterator wrapper_params_v = find_if(bn_map_full.begin(),
      bn_map_full.end(),
      BayesianNetwork::CompareVertexName("ProbabilitiesWrapperBag"));
  BooleanRandomVariable wrapper("Wrapper", true);
  BayesianNetwork::iterator wrapper_v = bn_map_full.add_conditional_categorical(
      wrapper, list_of(bag_v), wrapper_params_v);

  BayesianNetwork::iterator flavor_params_v = find_if(bn_map_full.begin(),
      bn_map_full.end(),
      BayesianNetwork::CompareVertexName("ProbabilitiesFlavorBag"));
  BooleanRandomVariable flavor("Flavor", true);
  BayesianNetwork::iterator flavor_v = bn_map_full.add_conditional_categorical(
      flavor, list_of(bag_v), flavor_params_v);

  if (options_map["with-debug-output"].as<bool>())
    cout << bn_map_full << endl;

  unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
      unsigned int>();
  unsigned int collect_iterations = options_map["collect-iterations"].as<
      unsigned int>();
  cout << "Sample with " << burn_in_iterations << " burn in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  CategoricalDistribution prediction = bn_map_full.sample(flavor_v,
      burn_in_iterations, collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Predictive distribution of " << prediction.begin()->first.name()
      << " with sampling:" << prediction << endl;
}

void
test_latent_bag_net()
{
  cout << "Generate the bag network with all data but no bag observations\n";
  double duration;
  boost::timer t;
  BayesianNetwork bn = NetworkGenerator::gen_bag_net(5.0, false);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << endl;

  if (options_map["with-debug-output"].as<bool>())
    cout << bn << endl;

  BayesianNetwork::iterator bag_params_v = find_if(bn.begin(), bn.end(),
      BayesianNetwork::CompareVertexName("ProbabilitiesBag"));

  BooleanRandomVariable bag("Bag", true);
  BayesianNetwork::iterator bag_v = bn.add_categorical(bag, bag_params_v);

  unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
      unsigned int>();
  unsigned int collect_iterations = options_map["collect-iterations"].as<
      unsigned int>();
  cout << "Sample with " << burn_in_iterations << " burn in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  CategoricalDistribution bag_d = bn.sample(bag_v, burn_in_iterations,
      collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Predictive bag distribution with sampling:" << bag_d << endl;
}

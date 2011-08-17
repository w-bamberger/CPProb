/*
 * main.cpp
 *
 *  Created on: 13.05.2011
 *      Author: wbam
 */

#include "boolean_random_variable.hpp"
#include "discrete_joint_random_variable.hpp"
#include "network_generator.hpp"
#include "io_utils.hpp"
#include "utils.hpp"
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
using namespace vanet;

enum TestCase
{
  alarm_test, bag_test, hybrid_bag_test
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
  else
    vanet_throw_runtime_error("Wrong argument for test case: " << value << ".");

  return is;
}

program_options::variables_map options_map;

ostream&
put_out_probability_variables(ostream& os, BayesianNetwork& bn);

void
test_alarm_net();

void
test_bag_net();

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
  ("with-debug-output",
      program_options::value<bool>()->default_value(false)->zero_tokens(),
      "print detailed information about the result of every step") //
  ("test-case",
      program_options::value<TestCase>()->default_value(hybrid_bag_test),
      "Choose what to test");
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
    test_bag_net();
    break;

  default:
    vanet_throw_logic_error(
        "Invalid test found: " << options_map["test-case"].as<TestCase> () << ".");
    break;

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
  cout << "Duration: " << duration << "\n" << endl;
  if (options_map["with-debug-output"].as<bool>())
    cout << bn << endl;

  BayesianNetwork::iterator search_it = find_if(bn.begin(), bn.end(),
      BayesianNetwork::CompareVertexName("Burglary"));

  cout << "Enumerate\n";
  t.restart();
  CategoricalDistribution burglary_distribution = bn.enumerate(search_it);
  duration = t.elapsed();
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
  BayesianNetwork bn_map_full = NetworkGenerator::gen_bag_net(5.0);
  duration = t.elapsed();
  cout << "Duration: " << duration << "\n" << endl;

  if (options_map["with-debug-output"].as<bool>())
    cout << bn_map_full << endl;

  cout << "Learn MAP on full data\n";
  t.restart();
  bn_map_full.learn();
  duration = t.elapsed();
  cout << "Duration: " << duration << "\n";
  put_out_probability_variables(cout, bn_map_full);
  cout << endl;

  cout << "Learn ML on full data\n";
  BayesianNetwork bn_ml_full = NetworkGenerator::gen_bag_net(0.0);
  bn_ml_full.learn();
  put_out_probability_variables(cout, bn_ml_full);
  cout << endl;

  cout << "Learn MAP on partial data\n";
  BayesianNetwork bn_map_partial = NetworkGenerator::gen_bag_net(5.0, 5);
  bn_map_partial.learn();
  put_out_probability_variables(cout, bn_map_partial);
  cout << endl;

  cout << "Learn ML on partial data\n";
  BayesianNetwork bn_ml_partial = NetworkGenerator::gen_bag_net(0.0, 5);
  bn_ml_partial.learn();
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
  CategoricalDistribution hole_d = bn_map_full.sample(flavor_v,
      burn_in_iterations, collect_iterations);
  duration = t.elapsed();
  cout << "Computation duration: " << duration << "\n";
  cout << "Predictive hole distribution with sampling: " << hole_d << endl;
}

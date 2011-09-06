/*
 * main.cpp
 *
 *  Created on: 13.05.2011
 *      Author: wbam
 */

#include "NetworkGenerator.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include "../src-lib/IoUtils.hpp"
#include "../src-lib/Utils.hpp"
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
      program_options::value<unsigned int>()->default_value(5),
      "number of iterations samples of which are not saved") //
  ("collect-iterations",
      program_options::value<unsigned int>()->default_value(500),
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

class PutOutParameters : public static_visitor<>
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
    operator()(const N& node)
    {
    }

private:

  ostream& os_;

};

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

  CategoricalNode& burglary_node = bn.find<CategoricalNode>("Burglary");

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
  for_each(bn_map_full.begin(), bn_map_full.end(),
      make_apply_visitor_delayed(PutOutParameters(cout)));
  cout << endl;

  cout << "Learn ML on full data\n";
  BayesianNetwork bn_ml_full = NetworkGenerator::gen_bag_net(0.0, true);
  t.restart();
  bn_ml_full.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  for_each(bn_ml_full.begin(), bn_ml_full.end(),
      make_apply_visitor_delayed(PutOutParameters(cout)));
  cout << endl;

  cout << "Learn MAP on partial data\n";
  BayesianNetwork bn_map_partial = NetworkGenerator::gen_bag_net(5.0, true, 5);
  t.restart();
  bn_map_partial.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  for_each(bn_map_partial.begin(), bn_map_partial.end(),
      make_apply_visitor_delayed(PutOutParameters(cout)));
  cout << endl;

  cout << "Learn ML on partial data\n";
  BayesianNetwork bn_ml_partial = NetworkGenerator::gen_bag_net(0.0, true, 5);
  t.restart();
  bn_ml_partial.learn();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  for_each(bn_ml_partial.begin(), bn_ml_partial.end(),
      make_apply_visitor_delayed(PutOutParameters(cout)));
  cout << endl;

  cout << "Extend the network for prediction\n";

  DirichletNode& bag_params_v = bn_map_full.find<DirichletNode>(
      "ProbabilitiesBag");
  RandomBoolean bag("Bag", true);
  CategoricalNode& bag_v = bn_map_full.add_categorical(bag, bag_params_v);

  ConditionalDirichletNode& hole_params_v = bn_map_full.find<
      ConditionalDirichletNode>("ProbabilitiesHoleBag");
  RandomBoolean hole("Hole", true);
  ConditionalCategoricalNode& hole_v = bn_map_full.add_conditional_categorical(
      hole, list_of(&bag_v), hole_params_v);
  hole_v.is_evidence(true);

  ConditionalDirichletNode& wrapper_params_v = bn_map_full.find<
      ConditionalDirichletNode>("ProbabilitiesWrapperBag");
  RandomBoolean wrapper("Wrapper", true);
  ConditionalCategoricalNode& wrapper_v =
      bn_map_full.add_conditional_categorical(wrapper, list_of(&bag_v),
          wrapper_params_v);
  wrapper_v.is_evidence(true);

  ConditionalDirichletNode& flavor_params_v = bn_map_full.find<
      ConditionalDirichletNode>("ProbabilitiesFlavorBag");
  RandomBoolean flavor("Flavor", true);
  ConditionalCategoricalNode& flavor_v =
      bn_map_full.add_conditional_categorical(flavor, list_of(&bag_v),
          flavor_params_v);

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

  DirichletNode& bag_params_v = bn.find<DirichletNode>("ProbabilitiesBag");
  RandomBoolean bag("Bag", true);
  CategoricalNode& bag_v = bn.add_categorical(bag, bag_params_v);

  ConditionalDirichletNode& hole_params_v = bn.find<ConditionalDirichletNode>(
      "ProbabilitiesHoleBag");
  RandomBoolean hole("Hole", true);
  ConditionalCategoricalNode& hole_v = bn.add_conditional_categorical(hole,
      list_of(&bag_v), hole_params_v);
  hole_v.is_evidence(true);

  ConditionalDirichletNode& wrapper_params_v =
      bn.find<ConditionalDirichletNode>("ProbabilitiesWrapperBag");
  RandomBoolean wrapper("Wrapper", true);
  ConditionalCategoricalNode& wrapper_v = bn.add_conditional_categorical(
      wrapper, list_of(&bag_v), wrapper_params_v);
  wrapper_v.is_evidence(true);

  ConditionalDirichletNode& flavor_params_v = bn.find<ConditionalDirichletNode>(
      "ProbabilitiesFlavorBag");
  RandomBoolean flavor("Flavor", true);
  ConditionalCategoricalNode& flavor_v = bn.add_conditional_categorical(flavor,
      list_of(&bag_v), flavor_params_v);

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

/*
 * AlarmTest.cpp
 *
 *  Created on: 07.10.2011
 *      Author: wbam
 */

#include "NetworkGenerator.hpp"
#include <boost/program_options.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>

using namespace cpprob;
using namespace std;

extern boost::program_options::variables_map options_map;

BOOST_AUTO_TEST_CASE( alarm_test )
{
  double duration;
  cout << "Generate the hybrid alarm network\n";
  boost::timer t;
  BayesianNetwork bn = NetworkGenerator::gen_alarm_net();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n" << endl;
  BOOST_CHECK_MESSAGE(bn.size() == 10, "Network size: " << bn.size());
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

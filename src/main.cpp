/*
 * main.cpp
 *
 *  Created on: 13.05.2011
 *      Author: wbam
 */

#include "error.h"
#include "boolean_random_variable.h"
#include "csv_map_reader.h"
#include "discrete_joint_random_variable.h"
#include "graph_generator.h"
#include "io_utils.h"
#include "utils.h"
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
  alarm_test, hybrid_alarm_test, bag_test, hybrid_bag_test
};

std::istream&
operator>>(std::istream& is, TestCase& tc)
{
  std::string value;
  is >> value;

  if (value == "alarm_test")
    tc = alarm_test;
  else if (value == "hybrid_alarm_test")
    tc = hybrid_alarm_test;
  else if (value == "bag_test")
    tc = bag_test;
  else if (value == "hybrid_bag_test")
    tc = hybrid_bag_test;
  else
    vanet_throw_runtime_error("Wrong argument for test case: " << value << ".");

  return is;
}

program_options::variables_map options_map;

vector<ConditionalCountDistribution>
make_prior(const DiscreteBayesianNetwork& bn, size_t prior_count);

void
make_vertex_prior(DiscreteRandomVariable vertex_var,
    const DiscreteRandomVariable& parents_var,
    ConditionalCountDistribution& vertex_prior, size_t prior_count);

void
test_alarm_net();

void
test_bag_net();

void
test_hybrid_alarm_net();

void
test_hybrid_bag_net();

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

  case hybrid_alarm_test:
    test_hybrid_alarm_net();
    break;

  case hybrid_bag_test:
    test_hybrid_bag_net();
    break;

  default:
    vanet_throw_logic_error(
        "Invalid test found: " << options_map["test-case"].as<TestCase> () << ".");
    break;

    }

  return 0;
}

vector<ConditionalCountDistribution>
make_prior(const DiscreteBayesianNetwork& bn, size_t prior_count)
{
  vector<ConditionalCountDistribution> prior(num_vertices(bn));

  DiscreteBayesianNetwork::vertex_iterator vertex_it, vertex_end_it;
  tie(vertex_it, vertex_end_it) = vertices(bn);
  for (; vertex_it != vertex_end_it; ++vertex_it)
    {

    DiscreteRandomVariable::Range parents_range = parents_variable(bn,
        *vertex_it).value_range();
    if (parents_range.empty())
      {
        make_vertex_prior(bn[*vertex_it].random_variable, parents_range.begin(),
            prior.at(*vertex_it), prior_count);
      }
    else
      {
        for (DiscreteRandomVariable parents_var = parents_range.begin();
            parents_var != parents_range.end(); ++parents_var)
              {
          make_vertex_prior(bn[*vertex_it].random_variable, parents_var,
              prior.at(*vertex_it), prior_count);
        }
    }
}

return prior;
}

void
make_vertex_prior(DiscreteRandomVariable vertex_var,
const DiscreteRandomVariable& parents_var,
ConditionalCountDistribution& vertex_prior, size_t prior_count)
{
DiscreteRandomVariable::Range vertex_range = vertex_var.value_range();
for (vertex_var = vertex_range.begin(); vertex_var != vertex_range.end();
  ++vertex_var)
    {
vertex_prior[make_pair(vertex_var, parents_var)] = prior_count;
}
}

void
test_alarm_net()
{
double duration;
timer t;
cout << "Generate the discrete alarm network\n";
DiscreteBayesianNetwork alarmNetwork = GraphGenerator::gen_alarm_net();
duration = t.elapsed();
cout << "Duration: " << duration << "\n" << endl;

DiscreteBayesianNetwork::vertex_descriptor burglary_v =
DiscreteBayesianNetwork::null_vertex();
std::pair<DiscreteBayesianNetwork::vertex_iterator,
DiscreteBayesianNetwork::vertex_iterator> vertex_range = vertices(alarmNetwork);
for (DiscreteBayesianNetwork::vertex_iterator it = vertex_range.first;
it != vertex_range.second; ++it)
{
VertexProperties& vp = alarmNetwork[*it];
if (vp.random_variable.name() == "JohnCalls")
{
  vp.value_is_evidence = true;
  vp.random_variable = BooleanRandomVariable("JohnCalls", true);
}
else if (vp.random_variable.name() == "MaryCalls")
{
  vp.value_is_evidence = true;
  vp.random_variable = BooleanRandomVariable("MaryCalls", true);
}
else if (vp.random_variable.name() == "Burglary")
{
  burglary_v = *it;
}
}
if (options_map["with-debug-output"].as<bool>())
cout << alarmNetwork << endl;

cout << "Copy discrete alarm network\n";
t.restart();
DiscreteBayesianNetwork alarmNetwork2(alarmNetwork);
duration = t.elapsed();
cout << "Duration: " << duration << "\n" << endl;

cout << "Run enumerate\n";
t.restart();
CategoricalDistribution burglary_distribution = enumerate(alarmNetwork,
burglary_v);
duration = t.elapsed();
cout << "Duration: " << duration << "\n";
cout << "Burglary distribution with enumerate:";
for_each(burglary_distribution.begin(), burglary_distribution.end(),
StreamOut(cout, " ", ""));
cout << "\n" << endl;

unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
unsigned int>();
unsigned int collect_iterations = options_map["collect-iterations"].as<
unsigned int>();
cout << "Run gibbs_sampling with " << burn_in_iterations
<< " burn in iterations and " << collect_iterations << " collect iterations.\n";
t.restart();
burglary_distribution = gibbs_sampling(alarmNetwork2, burglary_v,
burn_in_iterations + collect_iterations);
duration = t.elapsed();
cout << "Duration: " << duration << "\n";
cout << "Burglary distribution with Gibbs sampling:";
for_each(burglary_distribution.begin(), burglary_distribution.end(),
StreamOut(cout, " ", ""));
cout << "\n" << endl;

test_hybrid_alarm_net();
}

void
test_bag_net()
{
typedef map<string, string> NamedAttributes;
typedef vector<NamedAttributes> Records;

double duration;
timer t;

cout << "Generate the discrete bag network\n";
DiscreteBayesianNetwork bag_net = GraphGenerator::gen_bag_net();
duration = t.elapsed();
cout << "Duration: " << duration << "\n" << endl;
if (options_map["with-debug-output"].as<bool>())
{
cout << "Empty net\n";
cout << bag_net << endl;
}

CsvMapReader reader("bag.csv");
Records full_data = reader.read_rows();
vector<ConditionalCountDistribution> prior = make_prior(bag_net, 5);
cout << endl;

cout << "ML on full data\n";
t.restart();
learn_multinomial_ml(bag_net, full_data);
duration = t.elapsed();
cout << "Duration: " << duration << endl;
cout << bag_net << endl;

cout << "MAP on full data\n";
t.restart();
learn_multinomial_map(bag_net, full_data, prior);
duration = t.elapsed();
cout << "Duration: " << duration << endl;
cout << bag_net << endl;

Records partial_data(full_data.begin(), full_data.begin() + 5);

cout << "ML on partial data\n";
t.restart();
learn_multinomial_ml(bag_net, partial_data);
duration = t.elapsed();
cout << "Duration: " << duration << endl;
cout << bag_net << endl;

cout << "MAP on partial data\n";
t.restart();
learn_multinomial_map(bag_net, partial_data, prior);
duration = t.elapsed();
cout << "Duration: " << duration << endl;
cout << bag_net << endl;

cout << endl;
test_hybrid_bag_net();
}

void
test_hybrid_alarm_net()
{
cout << "Generate the hybrid alarm network\n";
double duration;
boost::timer t;
HybridBayesianNetwork bn = GraphGenerator::gen_alarm_net_hybrid();
duration = t.elapsed();
cout << "Duration: " << duration << "\n" << endl;
if (options_map["with-debug-output"].as<bool>())
cout << bn << "\n";

HybridBayesianNetwork::iterator search_it = find_if(bn.begin(), bn.end(),
HybridBayesianNetwork::CompareVertexName("Burglary"));

unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
unsigned int>();
unsigned int collect_iterations = options_map["collect-iterations"].as<
unsigned int>();
cout << "Run gibbs_sampling with " << burn_in_iterations
<< " burn in iterations and " << collect_iterations << " collect iterations.\n";
t.restart();
CategoricalDistribution burglary_distribution = bn.gibbs_sampling(search_it,
burn_in_iterations, collect_iterations);
duration = t.elapsed();
cout << "Duration: " << duration << "\n";
cout << "Burglary distribution with Gibbs sampling: " << burglary_distribution;
cout << endl;
}

void
test_hybrid_bag_net()
{
cout << "Generate the hybrid bag network\n";
double duration;
boost::timer t;
HybridBayesianNetwork bn = GraphGenerator::gen_bag_net_hybrid();
duration = t.elapsed();
cout << "Duration: " << duration << "\n" << endl;

HybridBayesianNetwork::iterator bag_params_v = find_if(bn.begin(), bn.end(),
HybridBayesianNetwork::CompareVertexName("ProbabilitiesBag"));
bag_params_v->value_is_evidence(true);

BooleanRandomVariable bag("Bag", true);
HybridBayesianNetwork::iterator bag_v = bn.add_categorical(bag, bag_params_v);
bag_v->value_is_evidence(true);

HybridBayesianNetwork::iterator hole_params_v = find_if(bn.begin(), bn.end(),
HybridBayesianNetwork::CompareVertexName("ProbabilitiesHoleBag"));
BooleanRandomVariable hole("Hole", true);
HybridBayesianNetwork::iterator hole_v = bn.add_conditional_categorical(hole,
list_of(bag_v), hole_params_v);

HybridBayesianNetwork::iterator wrapper_params_v = find_if(bn.begin(), bn.end(),
HybridBayesianNetwork::CompareVertexName("ProbabilitiesWrapperBag"));
BooleanRandomVariable wrapper("Wrapper", true);
HybridBayesianNetwork::iterator wrapper_v = bn.add_conditional_categorical(
wrapper, list_of(bag_v), wrapper_params_v);

HybridBayesianNetwork::iterator flavor_params_v = find_if(bn.begin(), bn.end(),
HybridBayesianNetwork::CompareVertexName("ProbabilitiesFlavorBag"));
BooleanRandomVariable flavor("Flavor", true);
HybridBayesianNetwork::iterator flavor_v = bn.add_conditional_categorical(
flavor, list_of(bag_v), flavor_params_v);

 //  if (options_map["with-debug-output"].as<bool> ())
 //    cout << bn << endl;

unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
unsigned int>();
unsigned int collect_iterations = options_map["collect-iterations"].as<
unsigned int>();
cout << "Run gibbs_sampling with " << burn_in_iterations
<< " burn in iterations and " << collect_iterations << " collect iterations.\n";
t.restart();
CategoricalDistribution hole_d = bn.gibbs_sampling(flavor_v, burn_in_iterations,
collect_iterations);
duration = t.elapsed();
cout << "Computation duration: " << duration << "\n";
cout << "Predictive hole distribution with Gibbs sampling: " << hole_d << endl;
}

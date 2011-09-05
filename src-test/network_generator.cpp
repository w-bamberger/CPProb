/*
 * network_generator.cpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#include "network_generator.hpp"
#include "csv_map_reader.hpp"
#include "../src-lib/boolean_random_variable.hpp"
#include "../src-lib/discrete_joint_random_variable.hpp"
#include <boost/assign.hpp>
#include <boost/program_options.hpp>
#include <boost/variant/get.hpp>

using namespace boost;
using namespace boost::assign;
using namespace std;

extern program_options::variables_map options_map;

namespace cpprob
{

  NetworkGenerator::NetworkGenerator()
  {
  }

  NetworkGenerator::~NetworkGenerator()
  {
  }

  BayesianNetwork
  NetworkGenerator::gen_alarm_net()
  {
    typedef BayesianNetwork::iterator vertex_iterator;
    BayesianNetwork bn;
    const DiscreteJointRandomVariable no_parents;

    BooleanRandomVariable burglary("Burglary", true);
    CategoricalNode& burglary_node = bn.add_categorical(burglary);
    RandomProbabilities& burglary_probs = burglary_node.probabilities();
    burglary_probs.set(burglary, 0.001);
    burglary_probs.set(burglary.observation(false), 0.999);

    BooleanRandomVariable earthquake("Earthquake", true);
    CategoricalNode& earthquake_node = bn.add_categorical(earthquake);
    RandomProbabilities& earthquake_probs = earthquake_node.probabilities();
    earthquake_probs.set(earthquake, 0.002);
    earthquake_probs.set(earthquake.observation(false), 0.998);

    BooleanRandomVariable alarm("Alarm", true);

    RandomConditionalProbabilities alarm_params(alarm,
        DiscreteJointRandomVariable(burglary, earthquake));
    burglary.observation(true);
    earthquake.observation(true);
    alarm_params.set(alarm.observation(true),
        DiscreteJointRandomVariable(burglary, earthquake), 0.95);
    alarm_params.set(alarm.observation(false),
        DiscreteJointRandomVariable(burglary, earthquake), 0.05);
    earthquake.observation(false);
    alarm_params.set(alarm.observation(true),
        DiscreteJointRandomVariable(burglary, earthquake), 0.94);
    alarm_params.set(alarm.observation(false),
        DiscreteJointRandomVariable(burglary, earthquake), 0.06);
    burglary.observation(false);
    earthquake.observation(true);
    alarm_params.set(alarm.observation(true),
        DiscreteJointRandomVariable(burglary, earthquake), 0.29);
    alarm_params.set(alarm.observation(false),
        DiscreteJointRandomVariable(burglary, earthquake), 0.71);
    earthquake.observation(false);
    alarm_params.set(alarm.observation(true),
        DiscreteJointRandomVariable(burglary, earthquake), 0.001);
    alarm_params.set(alarm.observation(false),
        DiscreteJointRandomVariable(burglary, earthquake), 0.999);
    ConstantNode<RandomConditionalProbabilities>& alarm_params_node =
        bn.add_constant(alarm_params);

    ConditionalCategoricalNode& alarm_node = bn.add_conditional_categorical(
        alarm, list_of(&burglary_node)(&earthquake_node), alarm_params_node);

    BooleanRandomVariable john_calls("JohnCalls", true);
    ConditionalCategoricalNode& john_calls_node =
        bn.add_conditional_categorical(john_calls, list_of(&alarm_node));
    john_calls_node.is_evidence(true);
    RandomConditionalProbabilities& john_calls_probs =
        john_calls_node.probabilities();
    alarm.observation(true);
    john_calls_probs.set(john_calls.observation(true), alarm, 0.9);
    john_calls_probs.set(john_calls.observation(false), alarm, 0.1);
    alarm.observation(false);
    john_calls_probs.set(john_calls.observation(true), alarm, 0.05);
    john_calls_probs.set(john_calls.observation(false), alarm, 0.95);

    BooleanRandomVariable mary_calls("MaryCalls", true);
    ConditionalCategoricalNode& mary_calls_node =
        bn.add_conditional_categorical(mary_calls, list_of(&alarm_node));
    mary_calls_node.is_evidence(true);
    RandomConditionalProbabilities& mary_calls_probs =
        mary_calls_node.probabilities();
    alarm.observation(true);
    mary_calls_probs.set(mary_calls.observation(true), alarm, 0.7);
    mary_calls_probs.set(mary_calls.observation(false), alarm, 0.3);
    alarm.observation(false);
    mary_calls_probs.set(mary_calls.observation(true), alarm, 0.01);
    mary_calls_probs.set(mary_calls.observation(false), alarm, 0.99);

    return bn;
  }

  BayesianNetwork
  NetworkGenerator::gen_bag_net(float alpha, bool fully_observed,
      size_t lines_of_evidence)
  {
    const DiscreteRandomReferences no_condition;
    BayesianNetwork bn;

    // Set up the parameter vertices
    map<string, ConditionalDirichletNode*> params_table;
    BooleanRandomVariable bag("Bag", true);
    RandomProbabilities bag_params(bag);
    DirichletNode& bag_params_node = bn.add_dirichlet(bag_params, alpha);
    RandomConditionalProbabilities flavor_params(
        BooleanRandomVariable("Flavor", true), bag);
    params_table.insert(
        make_pair("Flavor",
            &bn.add_conditional_dirichlet(flavor_params, alpha)));
    RandomConditionalProbabilities wrapper_params(
        BooleanRandomVariable("Wrapper", true), bag);
    params_table.insert(
        make_pair("Wrapper",
            &bn.add_conditional_dirichlet(wrapper_params, alpha)));
    RandomConditionalProbabilities hole_params(
        BooleanRandomVariable("Hole", true), bag);
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
      BooleanRandomVariable bag_evidence("Bag", r->operator[]("Bag"));
      CategoricalNode& bag_evidence_node = bn.add_categorical(bag_evidence,
          bag_params_node);
      if (fully_observed)
        bag_evidence_node.is_evidence(true);

      for (CsvMapReader::NamedAttributes::iterator a = r->begin();
          a != r->end(); ++a)
          {
        if (a->first != "Bag")
        {
          BooleanRandomVariable var(a->first, a->second);
          ConditionalCategoricalNode& node = bn.add_conditional_categorical(var,
              list_of(&bag_evidence_node), *params_table[a->first]);
          node.is_evidence(true);
        }
      }
    }

    return bn;
  }

}

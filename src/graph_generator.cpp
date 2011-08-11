/*
 * graph_generator.cpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#include "graph_generator.h"
#include "boolean_random_variable.h"
#include "csv_map_reader.h"
#include "discrete_joint_random_variable.h"
#include "io_utils.h"
#include <boost/assign.hpp>
#include <boost/variant/get.hpp>
#include <iostream>

using namespace boost;
using namespace boost::assign;
using namespace std;

namespace vanet
{

  GraphGenerator::GraphGenerator()
  {
  }

  GraphGenerator::~GraphGenerator()
  {
  }

  HybridBayesianNetwork
  GraphGenerator::gen_alarm_net()
  {
    typedef HybridBayesianNetwork::iterator vertex_iterator;
    HybridBayesianNetwork bn;
    const DiscreteJointRandomVariable no_parents;

    BooleanRandomVariable burglary("Burglary", true);
    vertex_iterator burglary_it = bn.add_categorical(burglary);
    CategoricalDistribution & burglary_d = get<CategoricalDistribution>(
        burglary_it->distribution());
    burglary_d.set(burglary, 0.001);
    burglary_d.set(burglary.observation(false), 0.999);

    BooleanRandomVariable earthquake("Earthquake", true);
    vertex_iterator earthquake_it = bn.add_categorical(earthquake);
    CategoricalDistribution & earthquake_d = get<CategoricalDistribution>(
        earthquake_it->distribution());
    earthquake_d.set(earthquake, 0.002);
    earthquake_d.set(earthquake.observation(false), 0.998);

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
    HybridBayesianNetwork::iterator alarm_params_it = bn.add_constant(
        alarm_params);
    alarm_params_it->value_is_evidence(true);

    HybridBayesianNetwork::iterator alarm_it = bn.add_conditional_categorical(
        alarm, list_of(burglary_it)(earthquake_it), alarm_params_it);

    BooleanRandomVariable john_calls("JohnCalls", true);
    vertex_iterator john_calls_it = bn.add_conditional_categorical(john_calls,
        list_of(alarm_it));
    john_calls_it->value_is_evidence(true);
    ConditionalCategoricalDistribution & john_calls_d = get<
        ConditionalCategoricalDistribution>(john_calls_it->distribution());
    alarm.observation(true);
    john_calls_d.set(john_calls.observation(true), alarm, 0.9);
    john_calls_d.set(john_calls.observation(false), alarm, 0.1);
    alarm.observation(false);
    john_calls_d.set(john_calls.observation(true), alarm, 0.05);
    john_calls_d.set(john_calls.observation(false), alarm, 0.95);

    BooleanRandomVariable mary_calls("MaryCalls", true);
    vertex_iterator mary_calls_it = bn.add_conditional_categorical(mary_calls,
        list_of(alarm_it));
    mary_calls_it->value_is_evidence(true);
    ConditionalCategoricalDistribution & mary_calls_d = get<
        ConditionalCategoricalDistribution>(mary_calls_it->distribution());
    alarm.observation(true);
    mary_calls_d.set(mary_calls.observation(true), alarm, 0.7);
    mary_calls_d.set(mary_calls.observation(false), alarm, 0.3);
    alarm.observation(false);
    mary_calls_d.set(mary_calls.observation(true), alarm, 0.01);
    mary_calls_d.set(mary_calls.observation(false), alarm, 0.99);

    return bn;
  }

  DiscreteBayesianNetwork
  GraphGenerator::gen_bag_net()
  {
    DiscreteBayesianNetwork g;
    const DiscreteJointRandomVariable no_parents;

    BooleanRandomVariable bag("Bag", true);
    DiscreteBayesianNetwork::vertex_descriptor bag_v = add_vertex(g);
    g[bag_v].random_variable = bag;

    BooleanRandomVariable flavor("Flavor", true);
    DiscreteBayesianNetwork::vertex_descriptor flavor_v = add_vertex(g);
    g[flavor_v].random_variable = flavor;

    add_edge(bag_v, flavor_v, g);

    BooleanRandomVariable wrapper("Wrapper", true);
    DiscreteBayesianNetwork::vertex_descriptor wrapper_v = add_vertex(g);
    g[wrapper_v].random_variable = wrapper;

    add_edge(bag_v, wrapper_v, g);

    BooleanRandomVariable hole("Hole", true);
    DiscreteBayesianNetwork::vertex_descriptor hole_v = add_vertex(g);
    g[hole_v].random_variable = hole;

    add_edge(bag_v, hole_v, g);

    return g;
  }

  HybridBayesianNetwork
  GraphGenerator::gen_bag_net_hybrid()
  {
    const HybridBayesianNetwork::VertexReferences no_condition;
    HybridBayesianNetwork bn;

    // Set up the parameter vertices
    map<string, HybridBayesianNetwork::iterator> params_table;
    BooleanRandomVariable bag("Bag", true);
    RandomProbabilities bag_params(bag);
    HybridBayesianNetwork::iterator bag_params_v = bn.add_constant(bag_params);
    bag_params_v->value_is_evidence(true);
    RandomConditionalProbabilities flavor_params(
        BooleanRandomVariable("Flavor", true), bag);
    params_table.insert(
        make_pair("Flavor", bn.add_conditional_dirichlet(flavor_params, 5.0)));
    RandomConditionalProbabilities wrapper_params(
        BooleanRandomVariable("Wrapper", true), bag);
    params_table.insert(
        make_pair("Wrapper",
            bn.add_conditional_dirichlet(wrapper_params, 5.0)));
    RandomConditionalProbabilities hole_params(
        BooleanRandomVariable("Hole", true), bag);
    params_table.insert(
        make_pair("Hole", bn.add_conditional_dirichlet(hole_params, 5.0)));

    // Read the data from the file
    CsvMapReader reader("bag.csv");
    CsvMapReader::Records full_data = reader.read_rows();

    // Fill the data in the net
    for (CsvMapReader::Records::iterator r = full_data.begin();
        r != full_data.end(); ++r)
        {
      BooleanRandomVariable bag_evidence("Bag", r->operator[]("Bag"));
      HybridBayesianNetwork::iterator bag_evidence_v = bn.add_categorical(
          bag_evidence, bag_params_v);
      bag_evidence_v->value_is_evidence(true);

      for (CsvMapReader::NamedAttributes::iterator a = r->begin();
          a != r->end(); ++a)
          {
        if (a->first != "Bag")
        {
          BooleanRandomVariable var(a->first, a->second);
          HybridBayesianNetwork::iterator vertex =
              bn.add_conditional_categorical(var, list_of(bag_evidence_v),
                  params_table[a->first]);
          vertex->value_is_evidence(true);
        }
      }
    }

    return bn;
  }

  DiscreteBayesianNetwork
  GraphGenerator::gen_naive_bayes_trust_net()
  {
    DiscreteBayesianNetwork g;

    BooleanRandomVariable trust("Trust", true);
    DiscreteBayesianNetwork::vertex_descriptor trust_v = add_vertex(g);
    g[trust_v].random_variable = trust;

    DiscreteBayesianNetwork::vertex_descriptor vehicle_v = add_vertex(g);
    DiscreteBayesianNetwork::vertex_descriptor type_v = add_vertex(g);

    bool success;
    success = add_edge(trust_v, vehicle_v, g).second;
    success = add_edge(trust_v, type_v, g).second;

    return g;
  }

}

/*
 * DirichletProcessTest.cpp
 *
 *  Created on: 10.10.2011
 *      Author: wbam
 */

#include "Test.hpp"
#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/DiscreteJointRandomVariable.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include "../src-lib/RandomInteger.hpp"
#include <boost/test/unit_test_monitor.hpp>
#include <iostream>

using namespace cpprob;
using namespace std;

class DirichletProcessFixture
{

public:

  DirichletProcessFixture()
  {

  }

};

BOOST_AUTO_TEST_SUITE(DirichletProcessTest)

template<class K, class V>
ostream&
put_out_map(ostream& os, const cont::map<K, V>& m)
{
  for (auto it = m.begin(); it != m.end(); ++it)
    os << it->first << ", " << it->second << "\n";
  return os;
}

BOOST_AUTO_TEST_CASE(Parameters)
{
  /* Test construction.
   *
   * There are two Dirichlet processes, DpCondition1 and DpCondition2. They
   * control the latent nodes of 4 observations. An observation consists of to
   * values: Value1 and Value2. Value1 depends on DpCondition1 and Value2 on
   * both, DpCondition1 and DpCondition2. For the second observation, only
   * Value1 exists (to make the network irregular). */
  RandomInteger condition1("DpCondition1", 1, 0);
  RandomInteger condition2("DpCondition2", 1, 0);
  RandomInteger condition12(DiscreteJointRandomVariable(condition1, condition2));
  RandomInteger value1("Value1", 5, 0);
  RandomBoolean value2("Value2", false);
  BayesianNetwork bn;
  typedef cont::map<
      DiscreteRandomVariable,
      DirichletProcessParameters::ConstChildren,
      DiscreteRandomVariable::NameLess> ChildLists;
  ChildLists child_lists1;
  ChildLists child_lists2;

  auto probabilities_node1 = bn.add_conditional_dirichlet(RandomConditionalProbabilities(value1, condition1), 1);
  auto probabilities_node2 = bn.add_conditional_dirichlet(RandomConditionalProbabilities(value2, condition12), 1);

  auto dp_parameters_node1 = bn.add_dirichlet_process_parameters("DpCondition1", 5,
      cont::vector<ConditionalDirichletNode*>(
          { &probabilities_node1, &probabilities_node2}));
  auto dp_parameters_node2 = bn.add_dirichlet_process_parameters("DpCondition2", 3,
      cont::vector<ConditionalDirichletNode*>(
          { &probabilities_node2}));

  auto infinite_mixture_node11 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node12 = bn.add_dirichlet_process(dp_parameters_node2);
  auto child1_node1 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node11}, probabilities_node1);
  child_lists1[infinite_mixture_node11.value()].push_back(&child1_node1);
  auto child2_node1 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node11, &infinite_mixture_node12}, probabilities_node2);
  child_lists2[infinite_mixture_node11.value()].push_back(&child2_node1);
  child_lists2[infinite_mixture_node12.value()].push_back(&child2_node1);

  auto infinite_mixture_node21 = bn.add_dirichlet_process(dp_parameters_node1);
  auto child1_node2 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node21}, probabilities_node1);
  child_lists1[infinite_mixture_node21.value()].push_back(&child1_node2);

  ++value1;
  ++(++value2);
  auto infinite_mixture_node31 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node32 = bn.add_dirichlet_process(dp_parameters_node2);
  auto child1_node3 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node31}, probabilities_node1);
  child_lists1[infinite_mixture_node31.value()].push_back(&child1_node3);
  auto child2_node3 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node31, &infinite_mixture_node32}, probabilities_node2);
  child_lists2[infinite_mixture_node31.value()].push_back(&child2_node3);
  child_lists2[infinite_mixture_node32.value()].push_back(&child2_node3);

  auto infinite_mixture_node41 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node42 = bn.add_dirichlet_process(dp_parameters_node2);
  auto child1_node4 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node41}, probabilities_node1);
  child_lists1[infinite_mixture_node41.value()].push_back(&child1_node4);
  auto child2_node4 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node41, &infinite_mixture_node42}, probabilities_node2);
  child_lists2[infinite_mixture_node41.value()].push_back(&child2_node4);
  child_lists2[infinite_mixture_node42.value()].push_back(&child2_node4);

  cout << bn;

  auto dp_parameters1 = dp_parameters_node1.value();
  RandomInteger old_condition1(condition1.name() + "_old", 0, 0);

  cout << "\nFill probabilities_node1" << endl;
  BOOST_TEST_CHECKPOINT("Fill probabilities_node1");
  cout << "probabilities_node1 before: " << probabilities_node1.value() << endl;
  dp_parameters1.extend_managed_node(probabilities_node1, old_condition1.value_range(), condition1.value_range(), child_lists1);
  cout << "probabilities_node1 afterwards: " << probabilities_node1.value() << endl;
  cout << "\nFill probabilities_node2" << endl;
  BOOST_TEST_CHECKPOINT("Fill probabilities_node2");
  cout << "probabilities_node2 before: " << probabilities_node2.value() << endl;
  dp_parameters1.extend_managed_node(probabilities_node2, old_condition1.value_range(), condition1.value_range(), child_lists2);
  cout << "probabilities_node2 afterwards: " << probabilities_node2.value() << endl;

  auto dp_parameters2 = dp_parameters_node2.value();
  RandomInteger old_condition2(condition2.name() + "_old", 0, 0);

  cout << "\nFill probabilities_node3 with parent 2" << endl;
  BOOST_TEST_CHECKPOINT("Fill probabilities_node2 with parent 2");
  dp_parameters2.extend_managed_node(probabilities_node2, old_condition2.value_range(), condition2.value_range(), child_lists2);
  cout << "probabilities_node2 afterwards: " << probabilities_node2.value() << endl;

  cout << "\nCreate new component of parent 1" << endl;
  dp_parameters1.create_component(
      { &child1_node4});
  cout << "probabilities_node1 afterwards: " << probabilities_node1.value() << endl;

  cout << "\nCreate new component of parent 2" << endl;
  dp_parameters2.create_component(
      { &child2_node4});
  cout << "probabilities_node2 afterwards: " << probabilities_node2.value() << endl;
}

BOOST_AUTO_TEST_CASE(Node)
{
  /* Test construction.
   *
   * There are two Dirichlet processes, DpCondition1 and DpCondition2. They
   * control the latent nodes of 4 observations. An observation consists of to
   * values: Value1 and Value2. Value1 depends on DpCondition1 and Value2 on
   * both, DpCondition1 and DpCondition2. For the second observation, only
   * Value1 exists (to make the network unregular). */
  RandomInteger condition1("DpCondition1", 1, 0);
  RandomInteger condition2("DpCondition2", 1, 0);
  RandomInteger condition12(DiscreteJointRandomVariable(condition1, condition2));
  RandomInteger value1("Value1", 5, 0);
  RandomBoolean value2("Value2", false);
  BayesianNetwork bn;

  auto probabilities_node1 = bn.add_conditional_dirichlet(RandomConditionalProbabilities(value1, condition1), 1);
  auto probabilities_node2 = bn.add_conditional_dirichlet(RandomConditionalProbabilities(value2, condition12), 1);

  auto dp_parameters_node1 = bn.add_dirichlet_process_parameters("DpCondition1", 2,
      cont::vector<ConditionalDirichletNode*>(
          { &probabilities_node1, &probabilities_node2}));
  auto dp_parameters_node2 = bn.add_dirichlet_process_parameters("DpCondition2", 1,
      cont::vector<ConditionalDirichletNode*>(
          { &probabilities_node2}));

  auto infinite_mixture_node11 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node12 = bn.add_dirichlet_process(dp_parameters_node2);
  auto child1_node1 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node11}, probabilities_node1);
  auto child2_node1 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node11, &infinite_mixture_node12}, probabilities_node2);

  auto infinite_mixture_node21 = bn.add_dirichlet_process(dp_parameters_node1);
  auto child1_node2 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node21}, probabilities_node1);

  ++value1;
  ++(++value2);
  auto infinite_mixture_node31 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node32 = bn.add_dirichlet_process(dp_parameters_node2);
  auto child1_node3 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node31}, probabilities_node1);
  auto child2_node3 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node31, &infinite_mixture_node32}, probabilities_node2);

  auto infinite_mixture_node41 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node42 = bn.add_dirichlet_process(dp_parameters_node2);
  auto child1_node4 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node41}, probabilities_node1);
  auto child2_node4 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node41, &infinite_mixture_node42}, probabilities_node2);

  cout << bn;

  cout << "Init probabilities_node1" << endl;
  probabilities_node1.init_sampling();
  cout << probabilities_node1.value() << endl;
  cout << "Init probabilities_node2" << endl;
  probabilities_node2.init_sampling();
  cout << probabilities_node2.value() << endl;

  put_out_map(cout, dp_parameters_node1.value().component_counters_) << endl;
  infinite_mixture_node11.sample();
  put_out_map(cout, dp_parameters_node1.value().component_counters_) << endl;
  cout << probabilities_node1.value() << endl;
  infinite_mixture_node21.sample();
  put_out_map(cout, dp_parameters_node1.value().component_counters_) << endl;
  cout << probabilities_node1.value() << endl;
  infinite_mixture_node31.sample();
  put_out_map(cout, dp_parameters_node1.value().component_counters_) << endl;
  cout << probabilities_node1.value() << endl;
  infinite_mixture_node41.sample();
  put_out_map(cout, dp_parameters_node1.value().component_counters_) << endl;
  cout << probabilities_node1.value() << endl;

  put_out_map(cout, dp_parameters_node2.value().component_counters_) << endl;
  infinite_mixture_node12.sample();
  put_out_map(cout, dp_parameters_node2.value().component_counters_) << endl;
  cout << probabilities_node2.value() << endl;
  infinite_mixture_node32.sample();
  put_out_map(cout, dp_parameters_node2.value().component_counters_) << endl;
  cout << probabilities_node2.value() << endl;
  infinite_mixture_node42.sample();
  put_out_map(cout, dp_parameters_node2.value().component_counters_) << endl;
  cout << probabilities_node2.value() << endl;
}

BOOST_AUTO_TEST_SUITE_END()

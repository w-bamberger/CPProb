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

class SimpleDirichletProcessFixture
{

public:

  const double concentration;
  BayesianNetwork bn;
  ConstantDirichletProcessParametersNode* dp_parameters_node;
  DirichletProcessNode* mixture_node1;
  DirichletProcessNode* mixture_node2;
  DirichletProcessNode* mixture_node3;
  ConditionalCategoricalNode* observation_node1;
  ConditionalCategoricalNode* observation_node2;
  ConditionalCategoricalNode* observation_node3;
  ConditionalDirichletNode* observation_probabilities_node;

  SimpleDirichletProcessFixture()
      : concentration(1.0)
  {
    RandomInteger mixture_component("MixtureComponent", 1, 0);
    RandomInteger observation("Observation", 5, 0);
    observation_probabilities_node = &bn.add_conditional_dirichlet(
        RandomConditionalProbabilities(observation, mixture_component),
        concentration);
    dp_parameters_node = &bn.add_dirichlet_process_parameters(
        mixture_component.name(), concentration,
          { observation_probabilities_node });

    mixture_node1 = &bn.add_dirichlet_process(*dp_parameters_node);
    observation_node1 = &bn.add_conditional_categorical(observation,
      { mixture_node1 }, *observation_probabilities_node);
    observation_node1->is_evidence(true);

    mixture_node2 = &bn.add_dirichlet_process(*dp_parameters_node);
    observation_node2 = &bn.add_conditional_categorical(observation,
      { mixture_node2 }, *observation_probabilities_node);
    observation_node2->is_evidence(true);

    ++observation;
    mixture_node3 = &bn.add_dirichlet_process(*dp_parameters_node);
    observation_node3 = &bn.add_conditional_categorical(observation,
      { mixture_node3 }, *observation_probabilities_node);
    observation_node3->is_evidence(true);
  }

};

BOOST_FIXTURE_TEST_SUITE(SimpleDirichletProcessTest, SimpleDirichletProcessFixture)

BOOST_AUTO_TEST_CASE(Construction)
{
  cout << bn << endl;

  BOOST_REQUIRE_EQUAL(dp_parameters_node->value().managed_nodes().size(), 1);
  BOOST_REQUIRE_EQUAL(&dp_parameters_node->value().managed_nodes().front(), observation_probabilities_node);
  BOOST_REQUIRE_EQUAL(dp_parameters_node->value().concentration(), concentration);
  BOOST_REQUIRE_EQUAL(dp_parameters_node->value().component_counters().size(), 0);
  BOOST_REQUIRE_EQUAL(dp_parameters_node->value().name(), "MixtureComponentParameters");
  BOOST_REQUIRE_EQUAL(dp_parameters_node->children().size(), 3);
  BOOST_REQUIRE_EQUAL(&dp_parameters_node->children()[0], mixture_node1);
  BOOST_REQUIRE_EQUAL(&dp_parameters_node->children()[1], mixture_node2);
  BOOST_REQUIRE_EQUAL(&dp_parameters_node->children()[2], mixture_node3);

  BOOST_CHECK_EQUAL(mixture_node1->children().size(), 1);
  BOOST_CHECK_EQUAL(&mixture_node1->children()[0], observation_node1);
  BOOST_CHECK_EQUAL(mixture_node1->is_evidence(), false);
  BOOST_CHECK_EQUAL(&mixture_node1->parameters(), &dp_parameters_node->value());
  BOOST_CHECK_EQUAL(mixture_node1->value(), RandomInteger("MixtureComponent", 1, 0));

  BOOST_CHECK_EQUAL(mixture_node2->children().size(), 1);
  BOOST_CHECK_EQUAL(&mixture_node2->children()[0], observation_node2);
  BOOST_CHECK_EQUAL(mixture_node2->is_evidence(), false);
  BOOST_CHECK_EQUAL(&mixture_node2->parameters(), &dp_parameters_node->value());
  BOOST_CHECK_EQUAL(mixture_node2->value(), RandomInteger("MixtureComponent", 1, 0));

  BOOST_CHECK_EQUAL(mixture_node3->children().size(), 1);
  BOOST_CHECK_EQUAL(&mixture_node3->children()[0], observation_node3);
  BOOST_CHECK_EQUAL(mixture_node3->is_evidence(), false);
  BOOST_CHECK_EQUAL(&mixture_node3->parameters(), &dp_parameters_node->value());
  BOOST_CHECK_EQUAL(mixture_node3->value(), RandomInteger("MixtureComponent", 1, 0));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DirichletProcessTest)

template<class M>
ostream&
put_out_map(ostream& os, const M& m)
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
   * control the latent nodes of 4 observations. An observation consists of two
   * values: Value1 and Value2. Value1 depends on DpCondition1 and Value2 on
   * both, DpCondition1 and DpCondition2. For the second observation, only
   * Value1 exists (to make the network irregular). */
  RandomInteger condition1("DpCondition1", 1, 0);
  RandomInteger condition2("DpCondition2", 1, 0);
  RandomInteger condition12(DiscreteJointRandomVariable(condition1, condition2));
  RandomInteger value1("Value1", 5, 0);
  RandomBoolean value2("Value2", false);
  BayesianNetwork bn;
  DirichletProcessParameters::ChildrenOfComponent child_lists1;
  DirichletProcessParameters::ChildrenOfComponent child_lists2;

  auto probabilities_node1 = bn.add_conditional_dirichlet(RandomConditionalProbabilities(value1, condition1), 1);
  auto probabilities_node2 = bn.add_conditional_dirichlet(RandomConditionalProbabilities(value2, condition12), 1);

#ifndef WITHOUT_INITIALIZER_LIST
  auto dp_parameters_node1 = bn.add_dirichlet_process_parameters("DpCondition1", 5,
      { &probabilities_node1, &probabilities_node2});
  auto dp_parameters_node2 = bn.add_dirichlet_process_parameters("DpCondition2", 3,
      { &probabilities_node2});
#else
  cont::vector<ConditionalDirichletNode*> managed_nodes;
  managed_nodes.push_back(&probabilities_node1);
  managed_nodes.push_back(&probabilities_node2);
  auto dp_parameters_node1 = bn.add_dirichlet_process_parameters("DpCondition1", 5,
      managed_nodes);
  managed_nodes.clear();
  managed_nodes.push_back(&probabilities_node2);
  auto dp_parameters_node2 = bn.add_dirichlet_process_parameters("DpCondition2", 3,
      managed_nodes);
#endif

  auto infinite_mixture_node11 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node12 = bn.add_dirichlet_process(dp_parameters_node2);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child1_node1 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node11}, probabilities_node1);
#else
  cont::RefVector<DiscreteNode> parents(1, infinite_mixture_node11);
  auto child1_node1 = bn.add_conditional_categorical(value1,
      parents, probabilities_node1);
#endif
  child_lists1[infinite_mixture_node11.value()].push_back(child1_node1);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child2_node1 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node11, &infinite_mixture_node12}, probabilities_node2);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node11);
  parents.push_back(infinite_mixture_node12);
  auto child2_node1 = bn.add_conditional_categorical(value2,
      parents, probabilities_node2);
#endif
  child_lists2[infinite_mixture_node11.value()].push_back(child2_node1);
  child_lists2[infinite_mixture_node12.value()].push_back(child2_node1);

  auto infinite_mixture_node21 = bn.add_dirichlet_process(dp_parameters_node1);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child1_node2 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node21}, probabilities_node1);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node21);
  auto child1_node2 = bn.add_conditional_categorical(value1,
      parents, probabilities_node1);
#endif
  child_lists1[infinite_mixture_node21.value()].push_back(child1_node2);

  ++value1;
  ++(++value2);
  auto infinite_mixture_node31 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node32 = bn.add_dirichlet_process(dp_parameters_node2);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child1_node3 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node31}, probabilities_node1);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node31);
  auto child1_node3 = bn.add_conditional_categorical(value1,
      parents, probabilities_node1);
#endif
  child_lists1[infinite_mixture_node31.value()].push_back(child1_node3);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child2_node3 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node31, &infinite_mixture_node32}, probabilities_node2);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node31);
  parents.push_back(infinite_mixture_node32);
  auto child2_node3 = bn.add_conditional_categorical(value2,
      parents, probabilities_node2);
#endif
  child_lists2[infinite_mixture_node31.value()].push_back(child2_node3);
  child_lists2[infinite_mixture_node32.value()].push_back(child2_node3);

  auto infinite_mixture_node41 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node42 = bn.add_dirichlet_process(dp_parameters_node2);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child1_node4 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node41}, probabilities_node1);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node41);
  auto child1_node4 = bn.add_conditional_categorical(value1,
      parents, probabilities_node1);
#endif
  child_lists1[infinite_mixture_node41.value()].push_back(child1_node4);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child2_node4 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node41, &infinite_mixture_node42}, probabilities_node2);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node41);
  parents.push_back(infinite_mixture_node42);
  auto child2_node4 = bn.add_conditional_categorical(value2,
      parents, probabilities_node2);
#endif
  child_lists2[infinite_mixture_node41.value()].push_back(child2_node4);
  child_lists2[infinite_mixture_node42.value()].push_back(child2_node4);

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
#ifndef WITHOUT_INITIALIZER_LIST
  dp_parameters1.create_component(
      { &child1_node4});
#else
  cont::RefVector<ConditionalCategoricalNode> children(1, child1_node4);
  dp_parameters1.create_component(children);
#endif
  cout << "probabilities_node1 afterwards: " << probabilities_node1.value() << endl;

  cout << "\nCreate new component of parent 2" << endl;
#ifndef WITHOUT_INITIALIZER_LIST
  dp_parameters2.create_component(
      { &child2_node4});
#else
  children.clear();
  children.push_back(child2_node4);
  dp_parameters2.create_component(children);
#endif
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

#ifndef WITHOUT_INITIALIZER_LIST
  auto dp_parameters_node1 = bn.add_dirichlet_process_parameters("DpCondition1", 2,
      cont::vector<ConditionalDirichletNode*>(
          { &probabilities_node1, &probabilities_node2}));
  auto dp_parameters_node2 = bn.add_dirichlet_process_parameters("DpCondition2", 1,
      cont::vector<ConditionalDirichletNode*>(
          { &probabilities_node2}));
#else
  cont::vector<ConditionalDirichletNode*> managed_nodes;
  managed_nodes.push_back(&probabilities_node1);
  managed_nodes.push_back(&probabilities_node2);
  auto dp_parameters_node1 = bn.add_dirichlet_process_parameters("DpCondition1", 5,
      managed_nodes);
  managed_nodes.clear();
  managed_nodes.push_back(&probabilities_node2);
  auto dp_parameters_node2 = bn.add_dirichlet_process_parameters("DpCondition2", 3,
      managed_nodes);
#endif

  auto infinite_mixture_node11 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node12 = bn.add_dirichlet_process(dp_parameters_node2);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child1_node1 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node11}, probabilities_node1);
  auto child2_node1 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node11, &infinite_mixture_node12}, probabilities_node2);
#else
  cont::RefVector<DiscreteNode> parents(1, infinite_mixture_node11);
  auto child1_node1 = bn.add_conditional_categorical(value1,
      parents, probabilities_node1);
  parents.push_back(infinite_mixture_node12);
  auto child2_node1 = bn.add_conditional_categorical(value2,
      parents, probabilities_node2);
#endif

  auto infinite_mixture_node21 = bn.add_dirichlet_process(dp_parameters_node1);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child1_node2 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node21}, probabilities_node1);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node21);
  auto child1_node2 = bn.add_conditional_categorical(value1,
      parents, probabilities_node1);
#endif

  ++value1;
  ++(++value2);
  auto infinite_mixture_node31 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node32 = bn.add_dirichlet_process(dp_parameters_node2);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child1_node3 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node31}, probabilities_node1);
  auto child2_node3 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node31, &infinite_mixture_node32}, probabilities_node2);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node31);
  auto child1_node3 = bn.add_conditional_categorical(value1,
      parents, probabilities_node1);
  parents.push_back(infinite_mixture_node32);
  auto child2_node3 = bn.add_conditional_categorical(value2,
      parents, probabilities_node2);
#endif

  auto infinite_mixture_node41 = bn.add_dirichlet_process(dp_parameters_node1);
  auto infinite_mixture_node42 = bn.add_dirichlet_process(dp_parameters_node2);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child1_node4 = bn.add_conditional_categorical(value1,
      { &infinite_mixture_node41}, probabilities_node1);
  auto child2_node4 = bn.add_conditional_categorical(value2,
      { &infinite_mixture_node41, &infinite_mixture_node42}, probabilities_node2);
#else
  parents.clear();
  parents.push_back(infinite_mixture_node41);
  auto child1_node4 = bn.add_conditional_categorical(value1,
      parents, probabilities_node1);
  parents.push_back(infinite_mixture_node42);
  auto child2_node4 = bn.add_conditional_categorical(value2,
      parents, probabilities_node2);
#endif

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

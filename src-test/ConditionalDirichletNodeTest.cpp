/*
 * ConditionalDirichletNodeTest.cpp
 *
 *  Created on: 10.10.2011
 *      Author: wbam
 */

#include "../src-lib/ConditionalDirichletNode.hpp"
#include "Test.hpp"
#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include "../src-lib/RandomInteger.hpp"
#include <ostream>

using namespace cpprob;
using namespace std;

BOOST_AUTO_TEST_SUITE(ConditionalDirichletNodeTest)

string formated_counts_list(const cont::map<DiscreteRandomVariable, ConditionalDirichletNode::Children>& child_lists)
{
  ostringstream oss;
  cont::map<DiscreteRandomVariable, cont::map<DiscreteRandomVariable, size_t> > counters;

  for (auto lists_it = child_lists.begin(); lists_it != child_lists.end(); ++lists_it)
  {
    for (auto value_it = lists_it->second.begin(); value_it != lists_it->second.end(); ++value_it)
    {
      counters[lists_it->first][value_it->value()] += 1;
    }
  }

  for (auto condition_it = counters.begin(); condition_it != counters.end(); ++condition_it)
  {
    oss << condition_it->first << ": ";
    for (auto value_it = condition_it->second.begin(); value_it != condition_it->second.end(); ++value_it)
    {
      oss << value_it->first << "#" << value_it->second << "  ";
    }
    oss << "\n";
  }

  return oss.str();
}

BOOST_AUTO_TEST_CASE(sampling)
{
  RandomInteger parent_value1("Parent", 3, 0);
  RandomInteger parent_value2(parent_value1);
  ++(++parent_value2);
  RandomBoolean child_value_true("Child", true);
  RandomBoolean child_value_false("Child", false);

  BayesianNetwork bn;
  cont::map<DiscreteRandomVariable, ConditionalDirichletNode::Children> child_lists;
  auto probabilities_node = bn.add_conditional_dirichlet(
      RandomConditionalProbabilities(child_value_true, parent_value1), 1);

  auto parent_node1 = bn.add_categorical(parent_value1);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child_node1 = bn.add_conditional_categorical(child_value_true,
      { &parent_node1}, probabilities_node);
#else
  cont::RefVector<DiscreteNode> parents(1, parent_node1);
  auto child_node1 = bn.add_conditional_categorical(child_value_true,
      parents, probabilities_node);
#endif
  child_lists[parent_node1.value()].push_back(child_node1);

  auto parent_node2 = bn.add_categorical(parent_value2);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child_node2 = bn.add_conditional_categorical(child_value_false,
      { &parent_node2}, probabilities_node);
#else
  parents.clear();
  parents.push_back(parent_node2);
  auto child_node2 = bn.add_conditional_categorical(child_value_false,
      parents, probabilities_node);
#endif
  child_lists[parent_node2.value()].push_back(child_node2);

  auto parent_node3 = bn.add_categorical(parent_value2);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child_node3 = bn.add_conditional_categorical(child_value_false,
      { &parent_node3}, probabilities_node);
#else
  parents.clear();
  parents.push_back(parent_node3);
  auto child_node3 = bn.add_conditional_categorical(child_value_false,
      parents, probabilities_node);
#endif
  child_lists[parent_node3.value()].push_back(child_node3);

  auto parent_node4 = bn.add_categorical(parent_value1);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child_node4 = bn.add_conditional_categorical(child_value_false,
      { &parent_node4}, probabilities_node);
#else
  parents.clear();
  parents.push_back(parent_node4);
  auto child_node4 = bn.add_conditional_categorical(child_value_false,
      parents, probabilities_node);
#endif
  child_lists[parent_node4.value()].push_back(child_node4);

  auto parent_node5 = bn.add_categorical(parent_value1);
#ifndef WITHOUT_INITIALIZER_LIST
  auto child_node5 = bn.add_conditional_categorical(child_value_true,
      { &parent_node5}, probabilities_node);
#else
  parents.clear();
  parents.push_back(parent_node5);
  auto child_node5 = bn.add_conditional_categorical(child_value_true,
      parents, probabilities_node);
#endif
  child_lists[parent_node5.value()].push_back(child_node5);

  /* Sample the regular probability table with the three possible conditions. */
  random_number_engine.seed();
  RandomInteger condition("Parent", 3, 0);
  for (; condition != condition.value_range().end(); ++condition)
  {
    probabilities_node.sample(condition, child_lists[condition]);
  }
  auto single_sampling = probabilities_node.value();

  random_number_engine.seed();
  probabilities_node.sample();
  auto full_sampling = probabilities_node.value();
  BOOST_CHECK_MESSAGE(single_sampling == full_sampling, "\nsingle_sampling (" << single_sampling << ") == \nfull_sampling (" << full_sampling << ") based on the counts:\n" << formated_counts_list(child_lists));

  /* Sample an addition fourth condition.
   * (Tests the automatic insertion of the additional condition. This is necessary for
   * infinite mixture models.) */
  random_number_engine.seed(1);
  RandomInteger extended_condition("Parent", 4, 0);
  for (; extended_condition != extended_condition.value_range().end(); ++extended_condition)
  {
    probabilities_node.sample(extended_condition, child_lists[extended_condition]);
  }
  single_sampling = probabilities_node.value();

  random_number_engine.seed(1);
  probabilities_node.sample();
  full_sampling = probabilities_node.value();
  BOOST_CHECK_MESSAGE(single_sampling == full_sampling, "\nsingle_sampling (" << single_sampling << ") == \nfull_sampling (" << full_sampling << ") based on the counts:\n" << formated_counts_list(child_lists));
}

BOOST_AUTO_TEST_SUITE_END()

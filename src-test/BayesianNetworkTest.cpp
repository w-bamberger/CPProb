/*
 * BayesianNetworkTest.cpp
 *
 *  Created on: 17.11.2011
 *      Author: wbam
 */

#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

using namespace cpprob;
using namespace std;

class BayesianNetworkFixture
{

protected:

  BayesianNetwork test_network_;

  BayesianNetworkFixture()
  {
    float alpha = 5.0;

    // Set up the parameter vertices
    RandomBoolean bag("Bag", true);
    RandomProbabilities bag_params(bag);
    auto* bag_params_node = &test_network_.add_dirichlet(bag_params, alpha);
    auto* bag_node1 = &test_network_.add_categorical(bag, *bag_params_node);
    bag_node1->is_evidence(true);
    bag.observation(false);
    auto* bag_node2 = &test_network_.add_categorical(bag, *bag_params_node);
    bag_node2->is_evidence(true);

    RandomBoolean wrapper("Wrapper", true);
    RandomConditionalProbabilities wrapper_params(wrapper, bag);
    auto* wrapper_params_node = &test_network_.add_conditional_dirichlet(
        wrapper_params, alpha);
    auto* wrapper_node1 = &test_network_.add_conditional_categorical(wrapper,
      { bag_node1 }, *wrapper_params_node);
    wrapper_node1->is_evidence(true);
    auto* wrapper_node2 = &test_network_.add_conditional_categorical(wrapper,
      { bag_node2 }, *wrapper_params_node);
    wrapper_node2->is_evidence(true);

    RandomBoolean hole("Hole", true);
    RandomConditionalProbabilities hole_params(hole, bag);
    auto* hole_params_node = &test_network_.add_conditional_dirichlet(
        hole_params, alpha);
    auto* hole_node1 = &test_network_.add_conditional_categorical(hole,
      { bag_node1 }, *hole_params_node);
    hole_node1->is_evidence(true);
    auto* hole_node2 = &test_network_.add_conditional_categorical(hole,
      { bag_node2 }, *hole_params_node);
    hole_node2->is_evidence(true);
  }

};

BOOST_FIXTURE_TEST_SUITE(BayesianNetworkTest, BayesianNetworkFixture)

BOOST_AUTO_TEST_CASE(Erase)
{
  BayesianNetwork bn = test_network_;
  auto* wrapper_params_node = &bn.at<ConditionalDirichletNode>("ProbabilitiesWrapperBag");

  BOOST_CHECK_EQUAL(bn.size(), 9);
  BOOST_CHECK_EQUAL(wrapper_params_node->children().size(), 2);
  BOOST_CHECK_THROW(bn.erase(*wrapper_params_node), std::invalid_argument);

  auto* wrapper_node = &bn.at<ConditionalCategoricalNode>("Wrapper");
  BOOST_CHECK_EQUAL(bn.erase(*wrapper_node), 1);
  BOOST_CHECK_EQUAL(wrapper_params_node->children().size(), 1);
  BOOST_CHECK_EQUAL(bn.size(), 8);

  wrapper_node = &bn.at<ConditionalCategoricalNode>("Wrapper");
  BOOST_CHECK_EQUAL(bn.erase(*wrapper_node), 1);
  BOOST_CHECK_EQUAL(wrapper_params_node->children().size(), 0);
  BOOST_CHECK_EQUAL(bn.size(), 7);

  BOOST_CHECK_EQUAL(bn.erase(*wrapper_node), 0);
  BOOST_CHECK_EQUAL(bn.size(), 7);
  BOOST_CHECK_THROW(bn.at<ConditionalCategoricalNode>("Wrapper"), std::out_of_range);
  wrapper_node = 0;

  BOOST_CHECK_EQUAL(bn.erase(*wrapper_params_node), 1);
  BOOST_CHECK_EQUAL(bn.size(), 6);
  wrapper_params_node = 0;

  // Simply run a sampling task in the end to see that nothing crashes.
  auto& bag_node = bn.at<CategoricalNode>("Bag");
  BOOST_TEST_MESSAGE("Bag node that is sampled:");
  BOOST_TEST_MESSAGE(bag_node);
  bag_node.is_evidence(false);
  auto distribution = bn.sample(bag_node, 0, 500);
  BOOST_TEST_MESSAGE("Sampled distribution: " << distribution);
  RandomBoolean bag("Bag", false);
  BOOST_CHECK_GT(distribution[bag.observation(false)], distribution[bag.observation(true)]);
}

BOOST_AUTO_TEST_SUITE_END()


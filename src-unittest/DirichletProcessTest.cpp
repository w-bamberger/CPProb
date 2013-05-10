/*
 * DirichletProcessTest.cpp
 *
 *  Created on: 10.10.2011
 *      Author: wbam
 */

#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/DiscreteJointRandomVariable.hpp"
#include "../src-lib/IoUtils.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include "../src-lib/RandomInteger.hpp"
#include "../src-lib/Test.hpp"
#include <boost/range/adaptor/map.hpp>
#include <boost/range/numeric.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_monitor.hpp>
#include <iostream>

using namespace boost::adaptors;
using namespace cpprob;
using namespace std;

template<class M>
  ostream&
  put_out_map(ostream& os, const M& m)
  {
    for (auto it = m.begin(); it != m.end(); ++it)
      os << it->first << ", " << it->second << "\n";
    return os;
  }

class SimpleDirichletProcessFixture
{

public:

  const double concentration;
  BayesianNetwork bn;
  ConstantDirichletProcessParametersNode* dp_parameters_node;
  DirichletProcessNode* mixture_node1;
  DirichletProcessNode* mixture_node2;
  DirichletProcessNode* mixture_node3;
  RandomInteger mixture_component;
  RandomInteger observation;
  ConditionalCategoricalNode* observation_node1;
  ConditionalCategoricalNode* observation_node2;
  ConditionalCategoricalNode* observation_node3;
  ConditionalDirichletNode* observation_probabilities_node;

  SimpleDirichletProcessFixture()
      : concentration(1.0), mixture_component("MixtureComponent", 0, 0), observation(
          "Observation", 5, 0)
  {
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

  void
  init_mixture_node1()
  {

    /* Creates a new component
     *
     * Mixture component:    The categorical distribution needs one random value.
     * Component parameters: Per observation value one Gamma distribution and
     *                       per Gamma distribution two random values. */
    random_number_engine.seed_from_canonical(
      { 0.99f, 0.6f, 0.7f, 0.1f, 0.2f, 0.05f, 0.1f, 0.4f, 0.5f, 0.6f, 0.7f });
    mixture_node1->init_sampling();
  }

  void
  init_mixture_node2()
  {
    /* Reuses an existing component
     *
     * Mixture component:    The categorical distribution needs one random value.
     * Component parameters: No sampling. */
    random_number_engine.seed_from_canonical(0.49f);
    mixture_node2->init_sampling();
  }

  void
  init_mixture_node3()
  {
    /* Create a new component
     *
     * Mixture component:    The categorical distribution needs one random value.
     * Component parameters: Per observation value one Gamma distribution and
     *                       per Gamma distribution two random values. */
    random_number_engine.seed_from_canonical(
      { 0.67f, 0.1f, 0.2f, 0.05f, 0.1f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.4f });
    mixture_node3->init_sampling();
  }

  void
  init_sampling()
  {
    observation_probabilities_node->init_sampling();
    init_mixture_node1();
    init_mixture_node2();
    init_mixture_node3();
    BOOST_REQUIRE_EQUAL(mixture_component.value_range().size(), 2);

    set_observation_probabilities_component_0();
    set_observation_probabilities_component_1();

    observation_node1->value() = observation.observation(1);
    observation_node2->value() = observation.observation(2);
    observation_node3->value() = observation.observation(3);
  }

  void
  set_observation_probabilities_component_0()
  {
    mixture_component.observation(0);

    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(0)) = 0.125;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(1)) = 0.125;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(2)) = 0.25;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(3)) = 0.25;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(4)) = 0.25;
  }

  void
  set_observation_probabilities_component_1()
  {
    mixture_component.observation(1);

    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(0)) = 0.25;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(1)) = 0.25;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(2)) = 0.25;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(3)) = 0.125;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(4)) = 0.125;
  }

  void
  set_observation_probabilities_component_2()
  {
    mixture_component.observation(2);

    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(0)) = 0.25;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(1)) = 0.25;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(2)) = 0.125;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(3)) = 0.125;
    observation_probabilities_node->value().at(mixture_component).at(
        observation.observation(4)) = 0.25;
  }

};

BOOST_FIXTURE_TEST_SUITE(SimpleDirichletProcessTest, SimpleDirichletProcessFixture)

void check_categorical_distribution(const CategoricalDistribution& actual,
    initializer_list<pair<const DiscreteRandomVariable, float> > unnormalised_target)
{
  auto& actual_distribution = actual;
  CategoricalDistribution target_distribution = unnormalised_target;
  target_distribution.normalize();
  CPPROB_CHECK_EQUAL_RANGES(actual_distribution, target_distribution);
}

void check_mixture(const ConstantDirichletProcessParametersNode& parameters_node,
    const DirichletProcessNode& mixture_node,
    const RandomInteger& component_value,
    initializer_list<size_t> component_counters)
{
  CPPROB_CHECK_EQUAL(mixture_node.value(), component_value);
  CPPROB_CHECK_EQUAL(mixture_node.value().value_range().size(), component_counters.size());

  auto actual_it = parameters_node.value().component_counters().begin();
  auto actual_end = parameters_node.value().component_counters().end();
  auto target_it = component_counters.begin();
  auto target_end = component_counters.end();
  auto mixture_component = component_value.value_range().begin();
  auto mixture_component_end = component_value.value_range().end();

  for (; actual_it != actual_end && target_it != target_end && mixture_component != mixture_component_end;
      ++actual_it, ++target_it, ++mixture_component)
  {
    BOOST_CHECK_MESSAGE(actual_it->first == mixture_component,
        "Mixture component continuous (" << actual_it->first << " == " << mixture_component << ")");

    BOOST_CHECK_MESSAGE(actual_it->second == *target_it,
        "Component counter correct (" << actual_it->second << " == " << *target_it << ")");
  }

  BOOST_CHECK_MESSAGE(actual_it == actual_end, "Reached end of actual component counters table");
  BOOST_CHECK_MESSAGE(mixture_component == mixture_component_end, "Reached end of mixture component range");
  BOOST_CHECK_MESSAGE(target_it == target_end, "Reached end of target component counters list");
}

void
check_number_generator_empty()
{
  BOOST_CHECK_EQUAL(random_number_engine.size(), 0);
}

BOOST_AUTO_TEST_CASE(Construction)
{
  BOOST_REQUIRE_EQUAL(mixture_component.value_range().size(), 0);
  mixture_component = RandomInteger(mixture_component.value_range().end());

  BOOST_CHECK_EQUAL(dp_parameters_node->value().managed_nodes().size(), 1);
  BOOST_CHECK_EQUAL(&dp_parameters_node->value().managed_nodes().front(), observation_probabilities_node);
  BOOST_CHECK_EQUAL(dp_parameters_node->value().concentration(), concentration);
  BOOST_CHECK_EQUAL(dp_parameters_node->value().component_counters().size(), 0);
  BOOST_CHECK(dp_parameters_node->value().component_counters().empty());
  BOOST_CHECK(dp_parameters_node->value().component_counters().begin() == dp_parameters_node->value().component_counters().end());
  BOOST_CHECK_EQUAL(dp_parameters_node->value().name(), "MixtureComponentParameters");
  BOOST_CHECK_EQUAL(dp_parameters_node->children().size(), 3);
  BOOST_CHECK_EQUAL(&dp_parameters_node->children()[0], mixture_node1);
  BOOST_CHECK_EQUAL(&dp_parameters_node->children()[1], mixture_node2);
  BOOST_CHECK_EQUAL(&dp_parameters_node->children()[2], mixture_node3);

  BOOST_CHECK_EQUAL(mixture_node1->children().size(), 1);
  BOOST_CHECK_EQUAL(&mixture_node1->children()[0], observation_node1);
  BOOST_CHECK_EQUAL(mixture_node1->is_evidence(), false);
  BOOST_CHECK_EQUAL(&mixture_node1->parameters(), &dp_parameters_node->value());
  BOOST_CHECK_EQUAL(mixture_node1->value().value_range().size(), 0);
  BOOST_CHECK_EQUAL(mixture_node1->value(), mixture_component);

  BOOST_CHECK_EQUAL(mixture_node2->children().size(), 1);
  BOOST_CHECK_EQUAL(&mixture_node2->children()[0], observation_node2);
  BOOST_CHECK_EQUAL(mixture_node2->is_evidence(), false);
  BOOST_CHECK_EQUAL(&mixture_node2->parameters(), &dp_parameters_node->value());
  BOOST_CHECK_EQUAL(mixture_node3->value().value_range().size(), 0);
  BOOST_CHECK_EQUAL(mixture_node3->value(), mixture_component);

  BOOST_CHECK_EQUAL(mixture_node3->children().size(), 1);
  BOOST_CHECK_EQUAL(&mixture_node3->children()[0], observation_node3);
  BOOST_CHECK_EQUAL(mixture_node3->is_evidence(), false);
  BOOST_CHECK_EQUAL(&mixture_node3->parameters(), &dp_parameters_node->value());
  BOOST_CHECK_EQUAL(mixture_node3->value().value_range().size(), 0);
  BOOST_CHECK_EQUAL(mixture_node3->value(), mixture_component);
}

BOOST_AUTO_TEST_CASE(InitSampling)
{
  auto& observation_probabilities = observation_probabilities_node->value();
  BOOST_REQUIRE_EQUAL(mixture_component.value_range().size(), 0);

  CPPROB_CHECKPOINT("Create the first component");

  check_categorical_distribution(mixture_node1->prior_distribution(),
      {
        { mixture_component, 1.0f}});

  init_mixture_node1();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node1, mixture_component,
      { 1});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 1);
  auto saved_probabilities_of_0 = observation_probabilities.at(mixture_component);

  CPPROB_CHECKPOINT("Reuse an existing component");

  check_categorical_distribution(mixture_node2->prior_distribution(),
      {
        { mixture_component, 0.5f},
        { RandomInteger(mixture_component.value_range().end()), 0.5f}});

  init_mixture_node2();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node2, mixture_component,
      { 2});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 1);
  BOOST_CHECK_EQUAL(observation_probabilities.at(mixture_component), saved_probabilities_of_0);

  CPPROB_CHECKPOINT("Take a new component");

  check_categorical_distribution(mixture_node3->prior_distribution(),
      {
        { mixture_component, 2.0f/3.0f},
        { mixture_component.value_range().end(), 1.0f/3.0f}});

  init_mixture_node3();
  mixture_component.observation(1);
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node3, mixture_component,
      { 2, 1});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 2);
  BOOST_CHECK_EQUAL(observation_probabilities.at(mixture_component.observation(0)), saved_probabilities_of_0);
}

BOOST_AUTO_TEST_CASE(Sampling)
{
  auto& component_counters = dp_parameters_node->value().component_counters();
  auto& observation_probabilities = observation_probabilities_node->value();

  /* Precondition are initialised nodes */
  init_sampling();

  auto mixture_component_0 = mixture_component.observation(0);
  auto mixture_component_1 = mixture_component.observation(1);
  auto mixture_component_end = mixture_component.value_range().end();

  BOOST_REQUIRE_EQUAL(component_counters.size(), 2);
  BOOST_REQUIRE_EQUAL(component_counters.at(mixture_component_0), 2);
  BOOST_REQUIRE_EQUAL(component_counters.at(mixture_component_1), 1);
  BOOST_REQUIRE_EQUAL(observation_probabilities.size(), 2);
  put_out_map(cout, observation_probabilities);

  CPPROB_CHECKPOINT("Reuse component");

  /*   CV |  CN |  OV |    OP
   *    0 |   1 |   1 | 0.125
   *    1 |   1 |   1 | 0.250
   *    ∞ |   1 |     | 0.200 (α = 1; β = 1 with 5 observation values: 1/(5*1))
   */
  check_categorical_distribution(mixture_node1->posterior_distribution(),
      {
        { mixture_component_0, 1.0 * 0.125},
        { mixture_component_1, 1.0 * 0.25},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(0.44);
  mixture_node1->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node1, mixture_component_1,
      { 1, 2});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 2);

  CPPROB_CHECKPOINT("Clear component 0 and reuse component 1");

  /*   CV |  CN |  OV |    OP
   *    0 |   0 |   2 | 0.250
   *    1 |   2 |   2 | 0.250
   *    ∞ |   1 |     | 0.200  */
  check_categorical_distribution(mixture_node2->posterior_distribution(),
      {
        { mixture_component_0, 0.0 * 0.25},
        { mixture_component_1, 2.0 * 0.25},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(0.00);
  mixture_node2->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node2, mixture_component_1,
      { 0, 3});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 2);

  CPPROB_CHECKPOINT("Reuse component");

  /*   CV |  CN |  OV |    OP
   *    0 |   0 |   3 | 0.250
   *    1 |   2 |   3 | 0.125
   *    ∞ |   1 |     | 0.200  */
  check_categorical_distribution(mixture_node3->posterior_distribution(),
      {
        { mixture_component_0, 0.0 * 0.25},
        { mixture_component_1, 2.0 * 0.125},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(0.55);
  mixture_node3->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node3, mixture_component_1,
      { 0, 3});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 2);

  BOOST_TEST_MESSAGE("\nNext sampling round\n");

  CPPROB_CHECKPOINT("Reactivate component 0");

  /*   CV |  CN |  OV |    OP
   *    0 |   0 |   1 | 0.125
   *    1 |   2 |   1 | 0.250
   *    ∞ |   1 |     | 0.200  */
  check_categorical_distribution(mixture_node1->posterior_distribution(),
      {
        { mixture_component_0, 0.0 * 0.125},
        { mixture_component_1, 2.0 * 0.25},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(
      { 0.72, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.3, 0.4, 0.2});
  mixture_node1->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node1, mixture_component_0,
      { 1, 2});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 2);

  set_observation_probabilities_component_0();

  /*   CV |  CN |  OV |    OP
   *    0 |   1 |   2 | 0.250
   *    1 |   1 |   2 | 0.250
   *    ∞ |   1 |     | 0.200  */
  CPPROB_CHECKPOINT("Create component 2");

  check_categorical_distribution(mixture_node2->posterior_distribution(),
      {
        { mixture_component_0, 1.0 * 0.25},
        { mixture_component_1, 1.0 * 0.25},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(
      { 0.9, 0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.4, 0.3, 0.2, 0.1});
  mixture_node2->sample();
  auto mixture_component_2 = mixture_component.observation(2);
  mixture_component_end = mixture_component.value_range().end();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node2, mixture_component_2,
      { 1, 1, 1});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 3);

  set_observation_probabilities_component_2();

  CPPROB_CHECKPOINT("Clear and reactivate component 1");

  /*   CV |  CN |  OV |    OP
   *    0 |   1 |   3 | 0.250
   *    1 |   0 |   3 | 0.125
   *    2 |   1 |   3 | 0.125
   *    ∞ |   1 |     | 0.200  */
  check_categorical_distribution(mixture_node3->posterior_distribution(),
      {
        { mixture_component_0, 1.0 * 0.25},
        { mixture_component_1, 0.0 * 0.125},
        { mixture_component_2, 1.0 * 0.125},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(
      { 0.66, 0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.4, 0.3, 0.2, 0.1});
  mixture_node3->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node3, mixture_component_1,
      { 1, 1, 1});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 3);

  set_observation_probabilities_component_1();

  BOOST_TEST_MESSAGE("\nNext sampling round\n");

  CPPROB_CHECKPOINT("Clear component 0 and reuse component 1");

  /*   CV |  CN |  OV |    OP
   *    0 |   0 |   1 | 0.125
   *    1 |   1 |   1 | 0.250
   *    2 |   1 |   1 | 0.250
   *    ∞ |   1 |     | 0.200  */
  check_categorical_distribution(mixture_node1->posterior_distribution(),
      {
        { mixture_component_0, 0.0 * 0.125},
        { mixture_component_1, 1.0 * 0.25},
        { mixture_component_2, 1.0 * 0.25},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(0.35);
  mixture_node1->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node1, mixture_component_1,
      { 0, 2, 1});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 3);

  CPPROB_CHECKPOINT("Clear component 2 and reuse component 1");

  /*   CV |  CN |  OV |    OP
   *    0 |   0 |   2 | 0.250
   *    1 |   2 |   2 | 0.250
   *    2 |   0 |   2 | 0.125
   *    ∞ |   1 |     | 0.200  */
  check_categorical_distribution(mixture_node2->posterior_distribution(),
      {
        { mixture_component_0, 0.0 * 0.25},
        { mixture_component_1, 2.0 * 0.25},
        { mixture_component_2, 0.0 * 0.125},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(0.71);
  mixture_node2->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node2, mixture_component_1,
      { 0, 3, 0});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 3);

  CPPROB_CHECKPOINT("Reactivate component 0");

  /*   CV |  CN |  OV |    OP
   *    0 |   0 |   3 | 0.250
   *    1 |   2 |   3 | 0.125
   *    2 |   0 |   3 | 0.125
   *    ∞ |   1 |     | 0.200  */
  check_categorical_distribution(mixture_node3->posterior_distribution(),
      {
        { mixture_component_0, 0.0 * 0.25},
        { mixture_component_1, 2.0 * 0.125},
        { mixture_component_2, 0.0 * 0.125},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(
      { 0.56, 0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.4, 0.3, 0.2, 0.1});
  mixture_node3->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node3, mixture_component_0,
      { 1, 2, 0});
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 3);

  set_observation_probabilities_component_0();

  BOOST_TEST_MESSAGE("\nNext sampling round\n");

  CPPROB_CHECKPOINT("Reactivate component 2");

  /*   CV |  CN |  OV |    OP
   *    0 |   1 |   1 | 0.125
   *    1 |   1 |   1 | 0.250
   *    2 |   0 |   1 | 0.250
   *    ∞ |   1 |     | 0.200  */
  check_categorical_distribution(mixture_node1->posterior_distribution(),
      {
        { mixture_component_0, 1.0 * 0.125},
        { mixture_component_1, 1.0 * 0.25},
        { mixture_component_2, 0.0 * 0.25},
        { mixture_component_end, 1.0 * 1.0 / 5.0}});

  random_number_engine.seed_from_canonical(
      { 0.66, 0.1, 0.2, 0.3, 0.4, 0.5, 0.5, 0.4, 0.3, 0.2, 0.1});
  mixture_node1->sample();
  check_number_generator_empty();
  check_mixture(*dp_parameters_node, *mixture_node1, mixture_component_2,
      { 1, 1, 1});
  BOOST_CHECK_EQUAL(mixture_node1->value(), mixture_component_2);
  BOOST_CHECK_EQUAL(mixture_component.value_range().size(), 3);
  BOOST_CHECK_EQUAL(component_counters.size(), 3);
  BOOST_CHECK_EQUAL(component_counters.at(mixture_component_0), 1);
  BOOST_CHECK_EQUAL(component_counters.at(mixture_component_1), 1);
  BOOST_CHECK_EQUAL(component_counters.at(mixture_component_2), 1);
  BOOST_CHECK_EQUAL(observation_probabilities.size(), 3);

  set_observation_probabilities_component_2();
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DirichletProcessTest)

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

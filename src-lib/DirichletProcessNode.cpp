/*
 * DirichletProcessNode.cpp
 *
 *  Created on: 29.09.2011
 *      Author: wbam
 */

#include "DirichletProcessNode.hpp"
#include "CategoricalDistribution.hpp"
#include "RandomInteger.hpp"
#include "RandomNumberEngine.hpp"

using namespace std;

namespace cpprob
{

  std::ostream&
  operator<<(std::ostream& os, const DirichletProcessNode& node)
  {
    os << "Dirichlet process node " << node.value().name() << " (at " << &node
        << ")" << " with current value " << node.value() << " (at "
        << &node.value() << ")\n";

    os << "  Parameter node: " << node.parameters().name() << " (at "
        << &node.parameters() << ")\n";

    os << "  Children: ";
    string prefix;
    for (auto c = node.children().begin(); c != node.children().end(); ++c)
    {
      os << prefix << c->value().name() << " (at " << &(*c) << ")";
      prefix = ", ";
    }
    os << "\n";

    return os;
  }

  DirichletProcessNode::DirichletProcessNode(
      DirichletProcessParameters& parameters)
      : DiscreteNode(RandomInteger(parameters.component_name_, 0, 0)), parameters_(
          parameters)
  {
  }

  void
  DirichletProcessNode::init_sampling()
  {
    DirichletProcessParameters::ComponentCounters& counters =
        parameters_.component_counters_;
    auto value_range_end = value().value_range().end();

    /* Set up the prior distribution of the node */
    CategoricalDistribution prior_distribution;
    copy(counters.begin(), counters.end(),
        inserter(prior_distribution, prior_distribution.begin()));
    prior_distribution[value_range_end] = parameters_.concentration();
    prior_distribution.normalize();

    /* Draw from the prior distribution. */
    variate_generator<RandomNumberEngine&, CategoricalDistribution> sampling_variate(
        random_number_engine, prior_distribution);
    DiscreteRandomVariable sample = sampling_variate();

    /* Create a new mixture component if necessary */
    if (sample != value_range_end)
      value() = sample;
    else
      value() = parameters_.create_component(children());

    /* Adjust the counters */
    counters[value()] += 1;

  }

  void
  DirichletProcessNode::sample()
  {
    DirichletProcessParameters::ComponentCounters& counters =
        parameters_.component_counters_;
    counters[value()] -= 1;

    /* Compile the posterior distribution for the used components. */
    CategoricalDistribution distribution;
    float p = 0.0;
    for (auto count = counters.begin(); count != counters.end(); ++count)
    {
      // The prior
      p = static_cast<float>(count->second);

      // The factors of the posterior update
      value() = count->first;
      for (auto c = children().begin(); c != children().end(); ++c)
      {
        p *= c->at_references();
      }

      auto insert_result = distribution.insert(make_pair(count->first, p));
      cpprob_check_debug(
          insert_result.second,
          "DirichletProcessNode: Could not insert a counter into the distribution during sampling.");
    }

    /* Compile the posterior distribution for the unused component. */
    // The prior
    p = parameters_.concentration();
    // The posterior update
    p *= parameters_.prior_probability_of_managed_nodes(children());
    auto value_range_end = counters.begin()->first.value_range().end();
    distribution[value_range_end] = p;

    distribution.normalize();

    /* Draw from the posterior distribution. */
    variate_generator<RandomNumberEngine&, CategoricalDistribution> sampling_variate(
        random_number_engine, distribution);
    DiscreteRandomVariable sample = sampling_variate();

    if (sample != value_range_end)
      value() = sample;
    else
      value() = parameters_.next_component(children());

    counters[value()] += 1;
  }

} /* namespace cpprob */

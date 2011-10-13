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
        << ")" << " with current value " << node.value_ << " (at "
        << &node.value() << ")\n";

    os << "  Parameter node: " << node.parameters().name() << " (at "
        << &node.parameters() << ")\n";

    os << "  Children: ";
    string prefix;
    for (auto c = node.children_.begin(); c != node.children_.end(); ++c)
    {
      os << prefix << (*c)->value().name() << " (at " << *c << ")";
      prefix = ", ";
    }
    os << "\n";

    return os;
  }

  DirichletProcessNode::DirichletProcessNode(
      DirichletProcessParameters& parameters)
      : parameters_(parameters), value_(
          RandomInteger(parameters.name(),
              parameters.component_counters_.size() + 1,
              parameters.component_counters_.size()))
  {
    parameters_.component_counters_[value_] += 1;
  }

  void
  DirichletProcessNode::init_sampling()
  {
  }

  void
  DirichletProcessNode::sample()
  {
    DirichletProcessParameters::ComponentCounters& counters =
        parameters_.component_counters_;
    counters[value_] -= 1;

    /* Compile the prior distribution. */
    CategoricalDistribution distribution;
    auto value_range_end = counters.begin()->first.value_range().end();
    for (auto count = counters.begin(); count != counters.end(); ++count)
      distribution.insert(*count);
    distribution[value_range_end] = parameters_.concentration();
    distribution.normalize();

    /* Draw from the prior distribution. */
    variate_generator<RandomNumberEngine&, CategoricalDistribution> sampling_variate(
        random_number_engine, distribution);
    DiscreteRandomVariable sample = sampling_variate();

    if (sample != value_range_end)
      value_ = sample;
    else
      value_ = parameters_.next_component(children_);

    counters[value_] += 1;
  }

} /* namespace cpprob */

/*
 * ConditionalCategoricalVertex.cpp
 *
 *  Created on: 30.08.2011
 *      Author: wbam
 */

#include "ConditionalCategoricalNode.hpp"

using namespace std;

namespace cpprob
{

  ostream&
  operator<<(ostream& os, const ConditionalCategoricalNode& node)
  {
    os << "ConditionalCategoricalNode " << node.value().name() << " (at "
        << &node << ")";
    if (node.is_evidence())
    {
      os << " with value\n";
      os << "  " << node.value() << " (at " << &node.value() << ")\n";
    }
    else
    {
      os << " with no evidence (at " << &node.value() << ")\n";
    }

    os << "  Parameter node: " << node.probabilities().name() << " (at "
        << &node.probabilities() << ")\n";
    os << "  Condition values: ";
    string prefix;
    for (DiscreteRandomReferences::const_iterator c = node.condition().begin();
        c != node.condition().end(); ++c)
    {
      os << prefix << c->name() << " (at " << &(*c) << ")";
      prefix = ", ";
    }
    os << "\n";

    os << "  Children: ";
    prefix = "";
    for (auto c = node.children().begin(); c != node.children().end(); ++c)
    {
      os << prefix << c->value().name() << " (at " << &(*c) << ")";
      prefix = ", ";
    }
    os << "\n";

    return os;
  }

  ConditionalCategoricalNode::ConditionalCategoricalNode(
      const DiscreteRandomVariable& value,
      const DiscreteRandomReferences& condition,
      RandomConditionalProbabilities& cpt)
      : DiscreteNode(value), is_evidence_(false), sampling_variate_(
          random_number_engine, CategoricalDistribution()), condition_(
          condition), probabilities_(cpt)
  {
  }

  ConditionalCategoricalNode::~ConditionalCategoricalNode()
  {
  }

  void
  ConditionalCategoricalNode::init_sampling()
  {
    /* Check requirements before accessing probabilities_.begin()->second. */
    cpprob_check_debug(
        probabilities_.size() != 0,
        "ConditionalCategoricalNode: Cannot initialize sampling from an empty conditional probability table.");

    /* Initialize value_ by drawing from the distribution given by the first
     * condition in probabilities_. This also initializes sampling_distribution.
     * ConditionalCategoricalNode::sample requires it to contain all possible
     * values of the DiscreteRandomVariable. */
    CategoricalDistribution& sampling_distribution =
        sampling_variate_.distribution();
    sampling_distribution.clear();
    RandomProbabilities& probabilities_subset = probabilities_.begin()->second;
    auto p_it = probabilities_subset.begin();
    for (; p_it != probabilities_subset.end(); ++p_it)
      sampling_distribution[p_it->first] = p_it->second;
    value() = sampling_variate_();
  }

  void
  ConditionalCategoricalNode::sample()
  {
    /* Get the variables and references that are necessary for all block below.
     * They come before the requirements tests because they are also needed
     * for these tests. */
    auto& sampling_distribution = sampling_variate_.distribution();
    auto d_end = sampling_distribution.end();
    auto& conditioned_probabilities = probabilities_.at(
        condition_.joint_value());

    /* Check requirements. */
    cpprob_check_debug(
        sampling_distribution.size() == conditioned_probabilities.size(),
        "ConditionalCategoricalNode: While sampling, the sampling distribution (size: " << sampling_distribution.size() << ") shows the wrong size compared to the conditional probability table(size: " << conditioned_probabilities.size() << ").");

    /* Initialize the sampling distribution with the prior. */
    auto d_it = sampling_distribution.begin();
    auto p_it = conditioned_probabilities.begin();
    for (; d_it != d_end; ++d_it, ++p_it)
      d_it->second = p_it->second;

    /* Update the sampling distribution with the likelihoods. */
    for (auto c = children().begin(); c != children().end(); ++c)
    {
      auto c_value = c->value();
      auto& c_probabilities = c->probabilities();
      auto c_condition = c->condition().sub_range(value()).begin();
      d_it = sampling_distribution.begin();

      for (; d_it != d_end; ++d_it, ++c_condition)
        d_it->second *= c_probabilities.at(c_condition.joint_value()).at(
            c_value);
    }
    sampling_distribution.normalize();

    /* Draw from the distribution. */
    value() = sampling_variate_();
  }

} /* namespace cpprob */

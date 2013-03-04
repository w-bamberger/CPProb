/*
 * CategoricalVertex.cpp
 *
 *  Created on: 30.08.2011
 *      Author: wbam
 */

#include "CategoricalNode.hpp"
#include "ConditionalCategoricalNode.hpp"

using namespace std;

namespace cpprob
{

  ostream&
  operator<<(ostream& os, const CategoricalNode& node)
  {
    os << "Categorical node " << node.value().name() << " (at " << &node << ")";
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
    os << "  Children: ";
    string prefix;
    for (auto c = node.children().begin(); c != node.children().end(); ++c)
    {
      os << prefix << c->value().name() << "(at " << &(*c) << ")";
      prefix = ", ";
    }
    os << "\n";

    return os;
  }

  CategoricalNode::CategoricalNode(const DiscreteRandomVariable& value,
      RandomProbabilities& probabilities)
      : DiscreteNode(value), is_evidence_(false), sampling_variate_(
          random_number_engine, CategoricalDistribution()), probabilities_(
          probabilities)
  {
  }

  CategoricalNode::~CategoricalNode()
  {
  }

  void
  CategoricalNode::init_sampling()
  {
    /* Initialize value_ by drawing from the distribution given by the
     * probability table. This also initializes sampling_distribution.
     * CategoricalNode::sample requires it to contain all possible values of
     * the DiscreteRandomVariable. */
    CategoricalDistribution& sampling_distribution =
        sampling_variate_.distribution();
    sampling_distribution.clear();
    for (auto p_it = probabilities_.begin(); p_it != probabilities_.end();
        ++p_it)
      sampling_distribution[p_it->first] = p_it->second;
    value() = sampling_variate_();
  }

  void
  CategoricalNode::sample()
  {
    /* Get the variables and references that are necessary for all block below.
     * They come before the requirements tests because they are also needed
     * for these tests. */
    CategoricalDistribution& sampling_distribution =
        sampling_variate_.distribution();
    auto d_end = sampling_distribution.end();

    /* Check requirements. */
    cpprob_check_debug(
        sampling_distribution.size() == probabilities_.size(),
        "CategoricalNode: While sampling, the sampling distribution (size: " << sampling_distribution.size() << ") shows the wrong size compared to the probability table(size: " << probabilities_.size() << ").");

    /* Initialize the sampling distribution with the prior. */
    auto d_it = sampling_distribution.begin();
    auto p_it = probabilities_.begin();
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

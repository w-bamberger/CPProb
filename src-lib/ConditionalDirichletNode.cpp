/*
 * ConditionalDirichletVertex.cpp
 *
 *  Created on: 30.08.2011
 *      Author: wbam
 */

#include "ConditionalDirichletNode.hpp"
#include "ConditionalCategoricalNode.hpp"
#include "DirichletDistribution.hpp"

using namespace std;

namespace cpprob
{

  ostream&
  operator<<(ostream& os, const ConditionalDirichletNode& node)
  {
    os << "ConditionalDirichletNode " << node.value().name();
    if (node.is_evidence())
    {
      os << " with value\n";
      os << "  " << node.value() << "\n";
    }
    else
    {
      os << " with no evidence\n";
    }

    string prefix;
    os << "  Parameters:";
    for (auto p = node.parameters().begin(); p != node.parameters().end();
        ++p)
    {
      os << prefix << "(" << p->first << ", " << p->second << ")";
      prefix = ", ";
    }
    os << "\n";

    os << "  Children: ";
    prefix = "";
    for (auto c = node.children().begin(); c != node.children().end(); ++c)
    {
      os << prefix << c->value().name() << "(at " << &(*c) << ")";
      prefix = ", ";
    }
    os << "\n";

    return os;
  }

  ConditionalDirichletNode::ConditionalDirichletNode(
      const RandomConditionalProbabilities& value, float alpha)
      : is_evidence_(false), value_(value)
  {
    /* Check the requirements. */
    if (value_.size() == 0)
      cpprob_throw_logic_error(
          "ConditionalDirichletNode: Cannot initialize a conditional Dirichlet node from an empty value.");
    RandomProbabilities& pt = value_.begin()->second;
    if (pt.size() == 0)
      cpprob_throw_logic_error(
          "ConditionalDirichletNode: Cannot initialize a conditional Dirichlet node from an empty value.");

    /* Initialize the parameter vector. */
    for (auto v_it = pt.begin(); v_it != pt.end(); ++v_it)
      parameters_[v_it->first] = alpha;
  }

  void
  ConditionalDirichletNode::init_sampling()
  {
    /* Check the requirements.
     * The range of the condition variable is taken from the children for the
     * case that the range changed after construction. This happens,
     * for example, in infinite mixture models.
     * Only if there are no children, I take the current value, which was given
     * to the constructor. */
    DiscreteRandomVariable::Range condition_range;
    if (children_.size() != 0)
    {
      condition_range =
          children().begin()->condition().joint_value().value_range();
    }
    else if (value_.size() != 0)
    {
      condition_range = value().begin()->first.value_range();
    }
    else
    {
      cpprob_throw_logic_error(
          "ConditionalDirichletNode: Cannot sample a conditional Dirichlet node (name: " + value_.name() + ") from an empty value, if there are no children.");
    }

    /* Set up the prior distribution. */
    DirichletDistribution sampling_distribution(parameters_.begin(),
        parameters_.end());
    variate_generator<RandomNumberEngine&, DirichletDistribution> sampling_variate_(
        random_number_engine, sampling_distribution);

    /* Sample the conditional probabilities */
    for (auto condition = condition_range.begin();
        condition != condition_range.end(); ++condition)
    {
      value_[condition] = sampling_variate_();
    }
  }

  void
  ConditionalDirichletNode::sample()
  {
    /* Check the requirements.
     * The range of the condition variable is taken from the children for the
     * case that the range changed after construction. This happens,
     * for example, in infinite mixture models.
     * Only if there are no children, I take the current value, which was given
     * to the constructor. */
    DiscreteRandomVariable::Range condition_range;
    if (children_.size() != 0)
    {
      condition_range =
          children().begin()->condition().joint_value().value_range();
    }
    else if (value_.size() != 0)
    {
      condition_range = value().begin()->first.value_range();
    }
    else
    {
      cpprob_throw_logic_error(
          "ConditionalDirichletNode: Cannot sample a conditional Dirichlet node (name: " + value_.name() + ") from an empty value, if there are no children.");
    }

    /* Set up the counters and initialize them with the Dirichlet prior. */
    DiscreteRandomVariableMap<Parameters> counters;
    for (auto condition = condition_range.begin();
        condition != condition_range.end(); ++condition)
    {
      counters.insert(make_pair(condition, parameters_));
    }

    /* Count the child values. */
    for (auto child = children_.begin(); child != children_.end(); ++child)
    {
      const DiscreteRandomVariable& child_var = child->value();
      const DiscreteRandomVariable& child_condition =
          child->condition().joint_value();
      counters[child_condition][child_var] += 1.0;
    }

    /* Set up the sampling distribution and draw from it. */
    for (auto counter_it = counters.begin(); counter_it != counters.end();
        ++counter_it)
    {
      DirichletDistribution sample_distribution(counter_it->second.begin(),
          counter_it->second.end());
      variate_generator<RandomNumberEngine&, DirichletDistribution> sampling_variate_(
          random_number_engine, sample_distribution);
      value_[counter_it->first] = sampling_variate_();
    }
  }

  void
  ConditionalDirichletNode::sample(const DiscreteRandomVariable& condition,
      const Children& children)
  {
    /* Set up the counters and initialize them with the Dirichlet prior. */
    Parameters counters = parameters_;

    /* Count the child value. */
    for (auto child = children.begin(); child != children.end(); ++child)
      counters[child->value()] += 1.0;

    /* Set up the sampling distribution and draw from it. */
    DirichletDistribution sample_distribution(counters.begin(), counters.end());
    std::variate_generator<RandomNumberEngine&, DirichletDistribution> sampling_variate_(
        random_number_engine, sample_distribution);
    value_[condition] = sampling_variate_();
  }

} /* namespace cpprob */

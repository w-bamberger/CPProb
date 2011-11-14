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
    for (ConditionalCategoricalNode::Children::const_iterator child =
        node.children_.begin(); child != node.children_.end(); ++child)
    {
      os << prefix << (*child)->value().name() << " (at " << *child << ")";
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
    CategoricalDistribution& sampling_distribution =
        sampling_variate_.distribution();
    sampling_distribution.clear();
    RandomProbabilities& probabilities_subset = probabilities_.begin()->second;
    RandomProbabilities::iterator p = probabilities_subset.begin();
    for (; p != probabilities_subset.end(); ++p)
      sampling_distribution[p->first] = p->second;
    value_ = sampling_variate_();
  }

  void
  ConditionalCategoricalNode::sample()
  {
    CategoricalDistribution& sampling_distribution =
        sampling_variate_.distribution();
    sampling_distribution.clear();

    DiscreteRandomVariable::Range x_range = value_.value_range();
    for (value_ = x_range.begin(); value_ != x_range.end(); ++value_)
    {
      float p = at_references();
      for (Children::const_iterator d = children_.begin(); d != children_.end();
          ++d)
      {
        p *= (*d)->at_references();
      }

      sampling_distribution[value_] = p;
    }

    sampling_distribution.normalize();
    value_ = sampling_variate_();
  }

} /* namespace cpprob */

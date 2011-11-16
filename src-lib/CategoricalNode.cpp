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
    for (auto c = node.children_.begin(); c != node.children_.end(); ++c)
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
    CategoricalDistribution& sampling_distribution =
        sampling_variate_.distribution();
    sampling_distribution.clear();
    RandomProbabilities::iterator p = probabilities_.begin();
    for (; p != probabilities_.end(); ++p)
      sampling_distribution[p->first] = p->second;
    value_ = sampling_variate_();
  }

  void
  CategoricalNode::sample()
  {
    CategoricalDistribution& sampling_distribution =
        sampling_variate_.distribution();
    sampling_distribution.clear();

    DiscreteRandomVariable::Range x_range = value_.value_range();
    for (value_ = x_range.begin(); value_ != x_range.end(); ++value_)
    {
      float p = at_references();
      for (auto c = children_.begin(); c != children_.end(); ++c)
      {
        p *= c->at_references();
      }

      sampling_distribution[value_] = p;
    }

    sampling_distribution.normalize();
    value_ = sampling_variate_();
  }

} /* namespace cpprob */

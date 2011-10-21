/*
 * DirichletVertex.cpp
 *
 *  Created on: 30.08.2011
 *      Author: wbam
 */

#include "DirichletNode.hpp"
#include "CategoricalNode.hpp"
#include "DirichletDistribution.hpp"

using namespace std;

namespace cpprob
{

  ostream&
  operator<<(ostream& os, const DirichletNode& node)
  {
    os << "DirichletNode " << node.value().name();
    if (node.is_evidence())
    {
      os << " with value\n";
      os << "  " << node.value() << "\n";
    }
    else
    {
      os << " with no evidence\n";
    }

    os << "  Parameters:";
    string prefix;
    for (auto p = node.parameters().begin(); p != node.parameters().end(); ++p)
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

  DirichletNode::DirichletNode(const RandomProbabilities& value, float alpha)
      : is_evidence_(false), value_(value)
  {
    for (RandomProbabilities::iterator v_it = value_.begin();
        v_it != value_.end(); ++v_it)
    {
      parameters_[v_it->first] = alpha;
    }
  }

  DirichletNode::~DirichletNode()
  {
  }

  void
  DirichletNode::init_sampling()
  {
    value_.clear();
    DirichletDistribution sampling_distribution(parameters_.begin(),
        parameters_.end());
    variate_generator<RandomNumberEngine&, DirichletDistribution> sampling_variate_(
        random_number_engine, sampling_distribution);
    value_ = sampling_variate_();
  }

  void
  DirichletNode::sample()
  {
    DirichletDistribution sample_distribution(parameters_.begin(),
        parameters_.end());

    for (Children::const_iterator c = children_.begin(); c != children_.end();
        ++c)
    {
      const DiscreteRandomVariable & c_var = (*c)->value();
      sample_distribution.parameters()[c_var] += 1.0;
    }

    variate_generator<RandomNumberEngine&, DirichletDistribution> sampling_variate_(
        random_number_engine, sample_distribution);
    value_ = sampling_variate_();
  }

} /* namespace cpprob */

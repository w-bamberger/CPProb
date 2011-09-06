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
using namespace std::tr1;

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

    os << "  Parameters: \n";
    return os;
  }

  ConditionalDirichletNode::ConditionalDirichletNode(
      const RandomConditionalProbabilities& value, float alpha)
      : is_evidence_(false), value_(value)
  {
    for (RandomConditionalProbabilities::iterator c_it = value_.begin();
        c_it != value_.end(); ++c_it)
        {
      RandomProbabilities& pt = c_it->second;
      map<DiscreteRandomVariable, float>& parameter_subset =
          parameters_[c_it->first];
      for (RandomProbabilities::iterator v_it = pt.begin(); v_it != pt.end();
          ++v_it)
          {
        parameter_subset[v_it->first] = alpha;
      }
    }
  }

  ConditionalDirichletNode::~ConditionalDirichletNode()
  {
  }

  void
  ConditionalDirichletNode::init_sampling()
  {
    value_.clear();
    for (Parameters::iterator p = parameters_.begin(); p != parameters_.end();
        ++p)
        {
      DirichletDistribution sampling_distribution(p->second.begin(),
          p->second.end());
      variate_generator<RandomNumberEngine&, DirichletDistribution> sampling_variate_(
          random_number_engine, sampling_distribution);
      value_[p->first] = sampling_variate_();
    }
  }

  void
  ConditionalDirichletNode::sample()
  {
    Parameters counters = parameters_;

    for (Children::iterator c = children_.begin(); c != children_.end(); ++c)
    {
      const DiscreteRandomVariable & c_var = (*c)->value();
      const DiscreteRandomVariable & c_condition =
          (*c)->condition().joint_value();
      counters[c_condition][c_var] += 1.0;
    }

    for (Parameters::iterator pc = counters.begin(); pc != counters.end(); ++pc)
    {
      DirichletDistribution sample_distribution(pc->second.begin(),
          pc->second.end());
      variate_generator<RandomNumberEngine&, DirichletDistribution> sampling_variate_(
          random_number_engine, sample_distribution);
      value_[pc->first] = sampling_variate_();
    }
  }

} /* namespace cpprob */

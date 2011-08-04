/*
 * categorical_distribution.cpp
 *
 *  Created on: 28.07.2011
 *      Author: wbam
 */

#include "categorical_distribution.h"

namespace vanet
{

  CategoricalDistribution::CategoricalDistribution() :
    is_pt_self_allocated(true), pt_(*new RandomProbabilities()), var_(0)
  {
  }

  CategoricalDistribution::CategoricalDistribution(
      const CategoricalDistribution& cd) :
    is_pt_self_allocated(cd.is_pt_self_allocated),
        pt_(make_parameters_from_other_cd(cd)), var_(cd.var_)
  {
  }

  CategoricalDistribution::CategoricalDistribution(DiscreteRandomVariable& var) :
    is_pt_self_allocated(true), pt_(*new RandomProbabilities(var)), var_(&var)
  {
  }

  CategoricalDistribution::CategoricalDistribution(DiscreteRandomVariable& var,
      RandomProbabilities& params) :
    is_pt_self_allocated(false), pt_(params), var_(&var)
  {
  }

  CategoricalDistribution::~CategoricalDistribution()
  {
    if (is_pt_self_allocated)
      delete &pt_;
  }

  float
  CategoricalDistribution::at_references() const
  {
    if (var_)
      return pt_.at(*var_);
    else
      throw std::runtime_error(
          "CategoricalDistribution: Access to reference in not-referenced distribution");
  }

  RandomProbabilities&
  CategoricalDistribution::make_parameters_from_other_cd(
      const CategoricalDistribution& cd)
  {
    if (cd.is_pt_self_allocated)
      return *new RandomProbabilities(cd.pt_);
    else
      return cd.pt_;
  }

  void
  CategoricalDistribution::normalize()
  {
    // Accumulate
    float sum = 0.0;
    for (iterator it = pt_.begin(); it != pt_.end(); ++it)
      {
        sum += it->second;
      }
    // Divide
    for (iterator it = pt_.begin(); it != pt_.end(); ++it)
      {
        it->second /= sum;
      }
  }

  std::ostream&
  CategoricalDistribution::put_out(std::ostream& os) const
  {
    for (const_iterator p = pt_.begin(); p != pt_.end(); ++p)
      os << " (" << p->first << "," << p->second << ") ";
    return os;
  }

  std::ostream&
  CategoricalDistribution::put_out_references(std::ostream& os) const
  {
    if (var_)
      return os << *var_;
    else
      throw std::runtime_error(
          "CategoricalDistribution: Cannot put out reference of not-referenced distribution.");
  }

}

/*
 * conditional_categorical_distribution.cpp
 *
 *  Created on: 08.07.2011
 *      Author: wbam
 */

#include "conditional_categorical_distribution.hpp"
#include "discrete_joint_random_variable.hpp"
#include "io_utils.hpp"
#include <algorithm>

namespace vanet
{

  ConditionalCategoricalDistribution::ConditionalCategoricalDistribution(
      DiscreteRandomVariable& var, const ConditionReferences& condition_vars) :
      cpt_(
          *new RandomConditionalProbabilities(var,
              make_condition(condition_vars))), condition_vars_(condition_vars), self_allocated_params_(
          true), var_(var)
  {

  }

  ConditionalCategoricalDistribution::ConditionalCategoricalDistribution(
      DiscreteRandomVariable& var, const ConditionReferences& condition_vars,
      RandomConditionalProbabilities& params) :
      cpt_(params), condition_vars_(condition_vars), self_allocated_params_(
          false), var_(var)
  {
  }

  ConditionalCategoricalDistribution::ConditionalCategoricalDistribution(
      const ConditionalCategoricalDistribution& ccd) :
      cpt_(make_parameters_from_other_ccd(ccd)), condition_vars_(
          ccd.condition_vars_), self_allocated_params_(
          ccd.self_allocated_params_), var_(ccd.var_)
  {

  }

  ConditionalCategoricalDistribution::~ConditionalCategoricalDistribution()
  {
    if (self_allocated_params_)
      delete &cpt_;
  }

  DiscreteRandomVariable
  ConditionalCategoricalDistribution::make_condition(
      const ConditionReferences& references)
  {
    DiscreteJointRandomVariable condition;
    for (ConditionReferences::const_iterator p_it = references.begin();
        p_it != references.end(); ++p_it)
        {
      condition.insert(**p_it);
    }
    return condition;

  }

  RandomConditionalProbabilities&
  ConditionalCategoricalDistribution::make_parameters_from_other_ccd(
      const ConditionalCategoricalDistribution& ccd)
  {
    if (ccd.self_allocated_params_)
      return *new RandomConditionalProbabilities(ccd.cpt_);
    else
      return ccd.cpt_;
  }

  std::ostream&
  ConditionalCategoricalDistribution::put_out_references(std::ostream& os) const
  {
    os << "  var: " << var_ << "\n";
    for (std::vector<const DiscreteRandomVariable*>::const_iterator p_it =
        condition_vars_.begin(); p_it != condition_vars_.end(); ++p_it)
      os << "  " << **p_it;
    os << "\n";
    return os;
  }

  std::ostream&
  ConditionalCategoricalDistribution::put_out(std::ostream& os) const
  {
    std::for_each(cpt_.begin(), cpt_.end(), StreamOut(os, "      ", "\n"));
    return os;
  }

}

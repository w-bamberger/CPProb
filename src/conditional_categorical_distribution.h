/*
 * conditional_probability_distribution.h
 *
 *  Created on: 24.05.2011
 *      Author: wbam
 */

#ifndef CONDITIONAL_CATEGORICAL_DISTRIBUTION_H_
#define CONDITIONAL_CATEGORICAL_DISTRIBUTION_H_

#include "bound_probability_distribution.h"
#include "random_conditional_probabilities.h"
#include <vector>

namespace vanet
{

  class ConditionalCategoricalDistribution : public BoundProbabilityDistribution
  {

  public:

    // Types following the map conventions
    typedef RandomConditionalProbabilities::const_iterator const_iterator;
    typedef RandomConditionalProbabilities::const_reference const_reference;
    typedef RandomConditionalProbabilities::iterator iterator;
    typedef RandomConditionalProbabilities::key_type key_type;
    typedef RandomConditionalProbabilities::mapped_type mapped_type;
    typedef RandomConditionalProbabilities::value_type value_type;

    // Types following the distribution conventions
    typedef float input_type;
    typedef DiscreteRandomVariable result_type;

    // Further custom types
    typedef std::vector<const DiscreteRandomVariable*> ConditionReferences;

    ConditionalCategoricalDistribution(DiscreteRandomVariable& var,
        const ConditionReferences& condition_vars);

    ConditionalCategoricalDistribution(DiscreteRandomVariable& var,
        const ConditionReferences& condition_vars,
        RandomConditionalProbabilities& params);

    ConditionalCategoricalDistribution(
        const ConditionalCategoricalDistribution& ccd);

    virtual
    ~ConditionalCategoricalDistribution();

    float
    at(const DiscreteRandomVariable& var,
        const DiscreteRandomVariable& condition)
    {
      return cpt_.at(var, condition);
    }

    virtual float
    at_references() const
    {
      return cpt_.at(var_, referenced_condition());
    }

    iterator
    begin()
    {
      return cpt_.begin();
    }

    const_iterator
    begin() const
    {
      return cpt_.begin();
    }

    void
    clear()
    {
      cpt_.clear();
    }

    iterator
    end()
    {
      return cpt_.end();
    }

    const_iterator
    end() const
    {
      return cpt_.end();
    }

    iterator
    insert(iterator position, const value_type& v)
    {
      return cpt_.insert(position, v);
    }

    template<class RandomNumberEngine>
      result_type
      operator()(RandomNumberEngine& rne)
      {
        input_type cum(0);
        const input_type r = rne();
        const RandomProbabilities& distribution = cpt_.at(
            referenced_condition());

        RandomProbabilities::const_iterator it = distribution.begin();
        for (; it != distribution.end(); ++it)
          {
            cum += it->second;
            if (cum >= r)
              return it->first;
          }
        --it;
        return it->first;
      }

    DiscreteRandomVariable
    referenced_condition() const
    {
      return make_condition(condition_vars_);
    }

    virtual std::ostream&
    put_out_references(std::ostream& os) const;

    void
    set(const DiscreteRandomVariable& var,
        const DiscreteRandomVariable& condition, float probability)
    {
      cpt_.set(var, condition, probability);
    }

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const;

  private:

    RandomConditionalProbabilities& cpt_;
    std::vector<const DiscreteRandomVariable*> condition_vars_;
    bool self_allocated_params_;
    DiscreteRandomVariable& var_;

    static DiscreteRandomVariable
    make_condition(const ConditionReferences& conditions);

    static RandomConditionalProbabilities&
    make_parameters_from_other_ccd(
        const ConditionalCategoricalDistribution& ccd);

    ConditionalCategoricalDistribution&
    operator=(const ConditionalCategoricalDistribution&);

  };

}

#endif /* CONDITIONAL_CATEGORICAL_DISTRIBUTION_H_ */

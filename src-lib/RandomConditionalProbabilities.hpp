/*
 * RandomConditionalProbabilities.hpp
 *
 *  Created on: 08.07.2011
 *      Author: wbam
 */

#ifndef RANDOMCONDITIONALPROBABILITIES_HPP_
#define RANDOMCONDITIONALPROBABILITIES_HPP_

#include "RandomProbabilities.hpp"

namespace cpprob
{

  class RandomConditionalProbabilities;

}

namespace std
{

  template<>
    struct is_floating_point<cpprob::RandomConditionalProbabilities> : public integral_constant<
        bool, true>
    {
    };

}

namespace cpprob
{

  class RandomConditionalProbabilities : public RandomVariable
  {

    typedef std::map<DiscreteRandomVariable, RandomProbabilities> ConditionalProbabilityTable;

  public:

    typedef ConditionalProbabilityTable::const_iterator const_iterator;
    typedef ConditionalProbabilityTable::const_reference const_reference;
    typedef ConditionalProbabilityTable::iterator iterator;
    typedef ConditionalProbabilityTable::key_type key_type;
    typedef ConditionalProbabilityTable::mapped_type mapped_type;
    typedef ConditionalProbabilityTable::value_type value_type;

    RandomConditionalProbabilities(const DiscreteRandomVariable& var,
        const DiscreteRandomVariable& condition);

    virtual
    ~RandomConditionalProbabilities();

    virtual void
    assign_random_value(RandomNumberEngine& rne);

    /// @todo should be removed
    float
    at(const DiscreteRandomVariable& var,
        const DiscreteRandomVariable& condition) const;

    const RandomProbabilities&
    at(const DiscreteRandomVariable& condition) const
    {
      ConditionalProbabilityTable::const_iterator f = cpt_.find(condition);
      if (f == cpt_.end())
        cpprob_throw_out_of_range(
            "RandomConditionalProbabilities: Could not find condition " << condition << " in the probability table.");
      return f->second;
    }

    RandomProbabilities&
    at(const DiscreteRandomVariable& condition)
    {
      ConditionalProbabilityTable::iterator f = cpt_.find(condition);
      if (f == cpt_.end())
        cpprob_throw_out_of_range(
            "RandomConditionalProbabilities: Could not find condition " << condition << " in the probability table.");
      return f->second;
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

    virtual const std::string&
    name() const
    {
      return name_;
    }

    void
    normalize()
    {
      for (iterator p = begin(); p != end(); ++p)
        p->second.normalize();
    }

    RandomProbabilities&
    operator[](const DiscreteRandomVariable& condition)
    {
      return cpt_[condition];
    }

    bool
    operator==(const RandomConditionalProbabilities& other) const
    {
      return cpt_ == other.cpt_;
    }

    bool
    operator!=(const RandomConditionalProbabilities& other) const
    {
      return !operator==(other);
    }

    void
    set(const DiscreteRandomVariable& var,
        const DiscreteRandomVariable& condition, float probability);

    std::size_t
    size() const
    {
      return cpt_.size();
    }

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const;

  private:

    std::string name_;
    ConditionalProbabilityTable cpt_;

  };

}

#endif /* RANDOMCONDITIONALPROBABILITIES_HPP_ */

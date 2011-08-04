/*
 * random_probabilities.h
 *
 *  Created on: 26.07.2011
 *      Author: wbam
 */

#ifndef RANDOM_PROBABILITIES_H_
#define RANDOM_PROBABILITIES_H_

#include "discrete_random_variable.h"
#include "error.h"
#include <set>

namespace vanet
{

  class RandomProbabilities;

}

namespace std
{
  namespace tr1
  {

    template<>
      struct is_floating_point<vanet::RandomProbabilities> : public integral_constant<
          bool, true>
      {
      };

  }
}

namespace vanet
{

  /**
   * Random vector that represents a probability table. It stores pairs of
   * discrete random variable values with their probabilities. The probabilities
   * are random variables though. In contrast, the discrete random variable
   * values are considered to be fixed.
   */
  class RandomProbabilities : public RandomVariable
  {

    typedef std::map<DiscreteRandomVariable, float> ProbabilityTable;

  public:

    typedef ProbabilityTable::const_iterator const_iterator;
    typedef ProbabilityTable::iterator iterator;

    explicit
    RandomProbabilities();

    explicit
    RandomProbabilities(const DiscreteRandomVariable& var);

    virtual
    ~RandomProbabilities();

    virtual void
    assign_random_value(RandomNumberEngine& rne);

    const float&
    at(const DiscreteRandomVariable& var) const
    {
      ProbabilityTable::const_iterator find_it = pt_.find(var);
      if (find_it != pt_.end())
        return find_it->second;
      else
        vanet_throw_out_of_range("RandomProbabilities: Unknown variable value " << var << ".");
    }

    float&
    at(const DiscreteRandomVariable& var)
    {
      ProbabilityTable::iterator find_it = pt_.find(var);
      if (find_it != pt_.end())
        return find_it->second;
      else
        vanet_throw_out_of_range("RandomProbabilities: Unknown variable value " << var << ".");
    }

    iterator
    begin()
    {
      return pt_.begin();
    }

    const_iterator
    begin() const
    {
      return pt_.begin();
    }

    void
    clear()
    {
      pt_.clear();
    }

    iterator
    end()
    {
      return pt_.end();
    }

    const_iterator
    end() const
    {
      return pt_.end();
    }

    iterator
    find(const DiscreteRandomVariable& var)
    {
      return pt_.find(var);
    }

    const_iterator
    find(const DiscreteRandomVariable& var) const
    {
      return pt_.find(var);
    }

    virtual const std::string&
    name() const
    {
      return *name_;
    }

    float&
    operator[](const DiscreteRandomVariable& var)
    {
      return pt_[var];
    }

    void
    set(const DiscreteRandomVariable& var, float probability)
    {
      pt_[var] = probability;
    }

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const;

  private:

    typedef std::set<std::string> Names;

    static Names names_;
    Names::const_iterator name_;
    ProbabilityTable pt_;

  };

}

#endif /* RANDOM_PROBABILITIES_H_ */

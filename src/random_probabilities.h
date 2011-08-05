/**
 * @file random_probabilities.h
 * Defines the class RandomProbabilities.
 *
 * @author Walter Bamberger
 *
 * @par License
 * Copyright (C) 2011 Walter Bamberger
 * @par
 * This file is part of CPProb.
 * @par
 * CPProb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version, with the
 * exceptions mentioned in the file LICENSE.txt.
 * @par
 * CPProb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * @par
 * You should have received a copy of the GNU Lesser General Public
 * License along with CPProb.  If not, see
 * <http://www.gnu.org/licenses/>.
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
  /** A class. Details */
  class Test
  {
    public:
      //@{
      /** Same documentation for both members. Details */
      void func1InGroup1();
      void func2InGroup1();
      //@}

      /** Function without group. Details. */
      void ungroupedFunction();
      void func1InGroup2();
    protected:
      void func2InGroup2();
  };

  void Test::func1InGroup1() {}
  void Test::func2InGroup1() {}

  /** @name Group2
   *  Description of group 2.
   */
  //@{
  /** Function 2 in group 2. Details. */
  void Test::func2InGroup2() {}
  /** Function 1 in group 2. Details. */
  void Test::func1InGroup2() {}
  //@}

  /*! \file
   *  docs for this file
   */

  //@{
  //! one description for all members of this group
  //! (because DISTRIBUTE_GROUP_DOC is YES in the config file)
  #define A 1
  #define B 2
  void glob_func();
  //@}

  /**
   * Random vector that represents a probability table. A probability
   * table usually shows in the left column all possible values
   * of a discrete random variable; they are the outcomes of an experiment.
   * The right column contains the probability assigned with the value of the
   * discrete random variable. RandomProbabilities represents such a table,
   * in which the probabilities in the right column are random variables.
   * These random variables are constraint in the way that they must sum up
   * to 1. So a certain object of this class is a certain realisation of the
   * random variable containing specific probability values that sum up to 1.
   *
   * A probability table can be well represented as a mapping from the value
   * of the outcome to its probability. So this class implements a map
   * interface as defined in the C++ Standard Template Library. In addition,
   * it has a name assigned and contains methods for output streaming and
   * random initialisation.
   */
  class RandomProbabilities : public RandomVariable
  {

    typedef std::map<DiscreteRandomVariable, float> ProbabilityTable;

  public:

    /**
     * Iterator type to access entries of the probability table as
     * constant elements.
     */
    typedef ProbabilityTable::const_iterator const_iterator;

    /**
     * Type of a reference to an constant element.
     */
    typedef ProbabilityTable::const_reference const_reference;

    /**
     * Integer type that can represent the difference between two iterators
     * of this class.
     */
    typedef ProbabilityTable::difference_type difference_type;

    /**
     * Iterator type to access entries of the probability table with
     * write access.
     */
    typedef ProbabilityTable::iterator iterator;

    /**
     * Type of the random variable value for which this class stores
     * probabilities.
     */
    typedef DiscreteRandomVariable key_type;

    /**
     * Type of the probabilities stored in this class.
     */
    typedef float mapped_type;

    /**
     * Type of a reference to an writable element.
     */
    typedef ProbabilityTable::reference reference;

    /**
     * Positive integer type that can represent the maximum number of
     * probabilities stored in this class.
     */
    typedef ProbabilityTable::size_type size_type;

    /**
     * Type of the tuple (random variable, probability) that represents a
     * row of a probability table.
     */
    typedef pair<const float, DiscreteRandomVariable> value_type;

    /**
     * Create an empty probability table. This constructor is not recommended,
     * because the probabilities of an empty table cannot sum up to one and
     * because the name of the random variable is undefined. For the use in
     * the class ProbabilityDistribution, this constructor is necessary though.
     * For this reason, the constructor is marked as explicit.
     *
     * @par Ensures:
     *  - <tt>#name() == ""</tt>.
     *  - <tt>#size() == 0</tt>.
     */
    explicit
    RandomProbabilities();

    /**
     * Create an equally distributed probability table for the given random
     * variable.
     *
     * @par Ensures:
     * - <tt>#name() == "Probabilities" + var.name()</tt>.
     * - The probability of every entry is <tt>1.0f / var.size()</tt>.
     */
    explicit
    RandomProbabilities(const DiscreteRandomVariable& var);

    /**
     * Cleans up.
     */
    virtual
    ~RandomProbabilities();

    virtual void
    assign_random_value(RandomNumberEngine& rne);

    ///@{
    /**
     * Provides a reference to the probability assigned with the value of @c var.
     *
     * @param var the event to which the probability should be returned
     * @return the probability of the event @c var
     * @throw std::out_of_range if @c var is not found in the probability table.
     */
    const float&
    at(const DiscreteRandomVariable& var) const
    {
      ProbabilityTable::const_iterator find_it = pt_.find(var);
      if (find_it == pt_.end())
        vanet_throw_out_of_range(
            "RandomProbabilities: Unknown variable value " << var << ".");

      return find_it->second;
    }

    float&
    at(const DiscreteRandomVariable& var)
    {
      ProbabilityTable::iterator find_it = pt_.find(var);
      if (find_it == pt_.end())
        vanet_throw_out_of_range(
            "RandomProbabilities: Unknown variable value " << var << ".");

      return find_it->second;
    }
    ///@}

    ///@{
    /**
     * Provides an iterator to the first row of the probability table. If the
     * probability table is empty (size() == 0), the place this iterator points
     * to is undefined.
     *
     * @par Ensures:
     *  - If size() == 0, begin() == end().
     *
     * @return the iterator to the first element
     * @throw None
     */
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
    //@}

    /**
     * Removes all entries from the probability table.
     *
     * @par Ensures:
     *  - size() == 0.
     *
     * @throw None
     */
    void
    clear()
    {
      pt_.clear();
    }

    ///@{
    /**
     * Provides an iterator behind the last row in the probability table. This
     * iterator may not be accessed. It always points to an invalid position.
     *
     * @par Ensures:
     *  - If size() == 0, end() == begin().
     *
     * @return an iterator behind the last element
     * @throw None
     */
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
    ///@}

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

    /**
     * The number of entries in this random vector.
     */
    size_type
    size() const
    {
      return pt_.size();
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

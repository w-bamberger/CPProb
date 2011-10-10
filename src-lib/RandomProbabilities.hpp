/**
 * @file RandomProbabilities.hpp
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
#ifndef RANDOMPROBABILITIES_HPP_
#define RANDOMPROBABILITIES_HPP_

#include "DiscreteRandomVariable.hpp"
#include "Error.hpp"
#include <set>

namespace cpprob
{

  class RandomProbabilities;

}

namespace std
{

    /**
     * Overload of type trait is_floating_point. std::variate_generator
     * requires that the generated object is of arithmetic type (i.e. either
     * integral or floating point). This definition sets the type verification
     * to true.
     */
    template<>
      struct is_floating_point<cpprob::RandomProbabilities> : public integral_constant<
          bool, true>
      {
      };

}

namespace cpprob
{

  /**
   * Random vector that represents a probability table. A probability
   * table usually shows in the left column all possible values
   * of a discrete random variable; they are the outcomes of an experiment.
   * The right column contains the probability assigned with the value of the
   * discrete random variable. %RandomProbabilities represents such a table,
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
    typedef std::pair<const DiscreteRandomVariable, float> value_type;

    /**
     * Create an empty probability table. This constructor is not recommended,
     * because the probabilities of an empty table cannot sum up to one and
     * because the name of the random variable is undefined. For the use in
     * the class ProbabilityDistribution, this constructor is necessary though.
     *
     * @par Ensures:
     * - <tt>%name() == ""</tt>.
     * - <tt>%size() == 0</tt>.
     *
     * @throw None
     */
    explicit
    RandomProbabilities();

    /**
     * Create an equally distributed probability table for the given random
     * variable.
     *
     * @par Ensures:
     * - <tt>%name() == "Probabilities" + var.name()</tt>.
     * - The probability of every entry is <tt>1.0f / var.size()</tt>.
     *
     * @param var The random variable for which an equally distributed
     *     probability table should be created
     * @throw None
     */
    explicit
    RandomProbabilities(const DiscreteRandomVariable& var);

    /**
     * Cleans up.
     *
     * @throw None
     */
    virtual
    ~RandomProbabilities();

    // Documented in the base class.
    virtual void
    assign_random_value(RandomNumberEngine& rne);

    /**
     * Provides a reference to the probability assigned with the value of @c var.
     *
     * @par Requires:
     * - The value @c var is listed in the probability table.
     *
     * @par Ensures:
     * - <tt>at(var) == *find(var)->second</tt>, if <tt>find(var) != %end()</tt>.
     * - The @c out_of_range exception is thrown, if <tt>find(var) == %end()</tt>.
     *
     * @param var the outcome for which the probability should be returned
     * @return the probability of the outcome @c var
     * @throw std::out_of_range if @c var is not found in the probability table.
     */
    const float&
    at(const DiscreteRandomVariable& var) const
    {
      ProbabilityTable::const_iterator find_it = pt_.find(var);
      if (find_it == pt_.end())
        cpprob_throw_out_of_range(
            "RandomProbabilities: Unknown variable value " << var << ".");

      return find_it->second;
    }

    /**
     * Provides a reference to the probability assigned with the value of @c var.
     *
     * @par Requires:
     * - The value @c var is listed in the probability table.
     *
     * @par Ensures:
     * - <tt>at(var) == *find(var)->second</tt>, if <tt>find(var) != %end()</tt>.
     * - The @c out_of_range exception is thrown, if <tt>find(var) == %end()</tt>.
     *
     * @param var the outcome for which the probability should be returned
     * @return the probability of the outcome @c var
     * @throw std::out_of_range if @c var is not found in the probability table.
     */
    float&
    at(const DiscreteRandomVariable& var)
    {
      ProbabilityTable::iterator find_it = pt_.find(var);
      if (find_it == pt_.end())
        cpprob_throw_out_of_range(
            "RandomProbabilities: Unknown variable value " << var << ".");

      return find_it->second;
    }

    /**
     * Provides an iterator to the first row of the probability table. If the
     * probability table is empty (<tt>%size() == 0</tt>), the place this iterator
     * points to is undefined.
     *
     * @par Ensures:
     *  - If <tt>%size() == 0</tt>, <tt>%begin() == %end()</tt>.
     *
     * @return the iterator to the first element
     * @throw None
     */
    iterator
    begin()
    {
      return pt_.begin();
    }

    /**
     * Provides an iterator to the first row of the probability table. If the
     * probability table is empty (<tt>%size() == 0</tt>), the place this iterator
     * points to is undefined.
     *
     * @par Ensures:
     *  - If <tt>%size() == 0</tt>, <tt>%begin() == %end()</tt>.
     *
     * @return the iterator to the first element
     * @throw None
     */
    const_iterator
    begin() const
    {
      return pt_.begin();
    }

    /**
     * Removes all entries from the probability table.
     *
     * @par Ensures:
     *  - %size() == 0.
     *
     * @throw None
     */
    void
    clear()
    {
      pt_.clear();
    }

    /**
     * Provides an iterator behind the last row in the probability table. This
     * iterator may not be accessed. It always points to an invalid position.
     *
     * @par Ensures:
     *  - If %size() == 0, %end() == %begin().
     *
     * @return an iterator behind the last element
     * @throw None
     */
    iterator
    end()
    {
      return pt_.end();
    }

    /**
     * Provides an iterator behind the last row in the probability table. This
     * iterator may not be accessed. It always points to an invalid position.
     *
     * @par Ensures:
     *  - If %size() == 0, %end() == %begin().
     *
     * @return an iterator behind the last element
     * @throw None
     */
    const_iterator
    end() const
    {
      return pt_.end();
    }

    /**
     * Looks up the entry in the probability table that matches
     * the outcome @c var.
     *
     * @par Ensures:
     * - If @c var is part of the probability table,
     *   <tt>find(var)->first == var</tt>.
     * - If @c var is not part of the probability table,
     *   <tt>find(var) == %end()</tt>.
     *
     * @param var the outcome to look up in this probability table
     * @return an iterator to the entry that matches @c var.
     * @throw None
     */
    iterator
    find(const DiscreteRandomVariable& var)
    {
      return pt_.find(var);
    }

    /**
     * Looks up the entry in the probability table that matches
     * the outcome @c var.
     *
     * @par Ensures:
     * - If @c var is part of the probability table,
     *   <tt>find(var)->first == var</tt>.
     * - If @c var is not part of the probability table,
     *   <tt>find(var) == %end()</tt>.
     *
     * @param var the outcome to look up in this probability table
     * @return an iterator to the entry that matches @c var.
     * @throw None
     */
    const_iterator
    find(const DiscreteRandomVariable& var) const
    {
      return pt_.find(var);
    }

    /**
     * Tries to insert a new row in this probability table. It only inserts
     * the row if its event (@c row->first) is not yet listed in the probability
     * table. Then it returns an iterator to the new row and the boolean
     * @em true. If the event @c row-first is already part of the probability
     * table, @em false and an iterator to the already existing row for the
     * given event is returned.
     *
     * This method is here for compatibility with standard maps. Maybe it is
     * easier to use #set() instead.
     *
     * @par Requires:
     * - <tt>0.0 <= row->second <= 1.0</tt>.
     *
     * @par Ensures:
     * - If @c row->first is not part of this probability table,
     *   @c insert(row) is equivalent to @c set(row->first, row->second).
     * - <tt>insert(row)->first->first == row->first</tt>
     * - If <tt>insert(row)->second == true</tt>,
     *   <tt>insert(row)->first == row</tt>
     *
     * @param row the event-probability pair to insert
     * @return an iterator to the event entry and and boolean that says whether
     *         a new entry has been created or not
     * @throw std::bad_alloc the memory for inserting a new element could
     *     not be requested.
     */
    std::pair<iterator, bool>
    insert(const value_type& row)
    {
      cpprob_check_debug( row.second >= 0.0f,
          "RandomProbability: Probability " << row.second << //
          " not inserted. A probability must be greater than 0.");
      cpprob_check_debug( row.second <= 1.0f,
          "RandomProbability: Probability " << row.second << //
          " not inserted. A probability must be smaller than 1.");
      return pt_.insert(row);
    }

    iterator
    insert(iterator position, const value_type& row)
    {
      return pt_.insert(position, row);
    }

    // Documented in the base class.
    virtual const std::string&
    name() const
    {
      return *name_;
    }

    void
    normalize();

    /**
     * Accesses the probability of the event @c var. If @c var is not part of
     * the probability table yet, it is inserted with probability 0. This method
     * returns a reference to the probability for read and write access.
     *
     * @par Ensures:
     * - If @c var is part of this probability table, <tt>operator[](var)</tt>,
     *   <tt>find(var)->second</tt> and <tt>at(var)</tt> provide all the same
     *   reference.
     * - If @c var is not part of the probability table and
     *   <tt>operator[](var)</tt> is called, <tt>find(var)->second</tt> is
     *   valid and returns 0.0f.
     *
     * @param var the event for which the probability should be found.
     * @return reference to the probability associated with the event @c var
     * @throw std::bad_alloc the memory for inserting a new element could
     *     not be requested.
     */
    float&
    operator[](const DiscreteRandomVariable& var)
    {
      return pt_[var];
    }

    bool
    operator==(const RandomProbabilities& other) const
    {
      return pt_ == other.pt_;
    }

    bool
    operator!=(const RandomProbabilities& other) const
    {
      return !operator==(other);
    }

    /**
     * Modifies the probability associated with the event @c var. If @c var
     * was not yet part of this probability table, it is inserted with the
     * given probability.
     *
     * This method is equivalent to operator[], except that it does not require
     * default constructability but copy constructability. It is here, to be
     * compatible with other distributions for which operator[] is not possible.
     *
     * @par Requires:
     * - <tt>0.0 <= probability <= 1.0</tt>
     *
     * @par Ensures:
     * - @c set(var,probability) is equivalent to
     *   <tt>(*this)[var] = probability</tt>.
     *
     * @param var the event for which the probability should be changed
     * @param probability the probability that should be associated with @c var
     * @throw std::bad_alloc the memory for inserting a new element could
     *     not be requested.
     */
    void
    set(const DiscreteRandomVariable& var, float probability)
    {
      cpprob_check_debug( probability >= 0.0f,
          "RandomProbability: Probability " << probability << //
          " not set. A probability must be greater than 0.");
      cpprob_check_debug( probability <= 1.0f,
          "RandomProbability: Probability " << probability << //
          " not set. A probability must be smaller than 1.");
      pt_[var] = probability;
    }

    /**
     * Provides the number of entries in this random vector.
     *
     * @par Ensures:
     * - If <tt>%begin() == %end()</tt>, this method returns 0.
     *
     * @return the number of components of this random vector
     * @throw None
     */
    size_type
    size() const
    {
      return pt_.size();
    }

  protected:

    // Documented in the base class.
    virtual std::ostream&
    put_out(std::ostream& os) const;

  private:

    typedef std::set<std::string> Names;

    static Names names_;
    Names::const_iterator name_;
    ProbabilityTable pt_;

  };

}

#endif /* RANDOMPROBABILITIES_HPP_ */

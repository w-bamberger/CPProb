/*
 * categorical_distribution.hpp
 *
 *  Created on: 16.05.2011
 *      Author: wbam
 */

#ifndef CATEGORICALDISTRIBUTION_HPP_
#define CATEGORICALDISTRIBUTION_HPP_

#include "DiscreteRandomVariableMap.hpp"

namespace cpprob
{

  class CategoricalDistribution
  {

    typedef DiscreteRandomVariableMap<float> ProbabilityTable;

  public:

    // Types following the map conventions
    typedef DiscreteRandomVariable key_type;
    typedef float mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;

    typedef ProbabilityTable::iterator iterator;
    typedef ProbabilityTable::const_iterator const_iterator;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    // Types following the distribution conventions
    typedef mapped_type input_type;
    typedef key_type result_type;

    CategoricalDistribution()
        : pt_()
    {
    }

#ifndef WITHOUT_INITIALIZER_LIST
    CategoricalDistribution(std::initializer_list<value_type> entries)
        : pt_(entries)
    {
    }
#endif

    // Use the implicit destructor, so the implicit copy and move operations
    // are generated by the compiler.

    const_iterator
    begin() const
    {
      return pt_.begin();
    }

    iterator
    begin()
    {
      return pt_.begin();
    }

    void
    clear()
    {
      pt_.clear();
    }

    const_iterator
    end() const
    {
      return pt_.end();
    }

    iterator
    end()
    {
      return pt_.end();
    }

    const_iterator
    find(const key_type& var) const
    {
      return pt_.find(var);
    }

    iterator
    find(const key_type& var)
    {
      return pt_.find(var);
    }

    iterator
    insert(iterator position, const value_type& new_entry)
    {
      return pt_.insert(position, new_entry);
    }

    std::pair<iterator, bool>
    insert(const value_type& new_entry)
    {
      return pt_.insert(new_entry);
    }

    void
    normalize();

    template<class _RandomNumberGenerator>
      result_type
      operator()(_RandomNumberGenerator& rng)
      {
        cpprob_check_debug(
            pt_.size() != 0,
            "CategoricalDistribution: Cannot sample from an empty probability table.");
        input_type cum(0);
        const input_type r = rng();
        iterator it = pt_.begin();

        /* @TODO When assuming that the operator() is called more often than
         * the parameters change, the cumulative distribution could be
         * pre-computed and stored in a member variable. Then a simple call
         * to something like std::lower_bound finds the right value.
         *
         * @TODO To make the code compatible with C++11, this method has to
         * accept any URNG. Use generate_canonical to achieve this. See the
         * implementation of discrete_distribution in random.tcc (GCC 4.8) and
         * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1933.pdf.
         */

        /* This loop is necessary in the case that r == 0 and
         * the first row(s) in the probability table have probability 0. */
        for (; it->second == 0 && it != pt_.end(); ++it)
          ;

        /* Now execute the inverse cumulative probability function. */
        for (; it != pt_.end(); ++it)
        {
          cum += it->second;
          if (cum >= r)
            return it->first;
        }
        --it;
        return it->first;
      }

    float&
    operator[](const key_type& r)
    {
      return pt_[r];
    }

#ifndef WITHOUT_INITIALIZER_LIST
    CategoricalDistribution&
    operator=(std::initializer_list<value_type> entries)
    {
      pt_ = entries;
      return *this;
    }
#endif

    std::size_t
    size() const
    {
      return pt_.size();
    }

  private:

    ProbabilityTable pt_;

  };

  std::vector<float>
  difference(const CategoricalDistribution& minuend,
      const CategoricalDistribution& subtrahend);

  float
  mean(const CategoricalDistribution& d);

  std::ostream&
  operator<<(std::ostream& os, const CategoricalDistribution& d);

  float
  standard_deviation(const CategoricalDistribution& d);

  float
  variance(const CategoricalDistribution& d);

}

#endif /* CATEGORICALDISTRIBUTION_HPP_ */

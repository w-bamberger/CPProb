/*
 * categorical_distribution.hpp
 *
 *  Created on: 16.05.2011
 *      Author: wbam
 */

#ifndef CATEGORICALDISTRIBUTION_HPP_
#define CATEGORICALDISTRIBUTION_HPP_

#include "DiscreteRandomVariable.hpp"

namespace cpprob
{

  class CategoricalDistribution
  {

    typedef std::map<DiscreteRandomVariable, float> ProbabilityTable;

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
    {
    }

    CategoricalDistribution(std::initializer_list<value_type> entries)
        : pt_(entries)
    {
    }

    ~CategoricalDistribution()
    {
    }

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
        input_type cum(0);
        const input_type r = rng();
        ProbabilityTable::const_iterator it = pt_.begin();

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

    CategoricalDistribution&
    operator=(std::initializer_list<value_type>& entries)
    {
      pt_ = entries;
      return *this;
    }

    std::size_t
    size() const
    {
      return pt_.size();
    }

  private:

    ProbabilityTable pt_;

  };

  std::ostream&
  operator<<(std::ostream& os, const CategoricalDistribution& d);

}

#endif /* CATEGORICALDISTRIBUTION_HPP_ */

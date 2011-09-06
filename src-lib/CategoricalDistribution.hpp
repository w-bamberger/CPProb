/*
 * categorical_distribution.hpp
 *
 *  Created on: 16.05.2011
 *      Author: wbam
 */

#ifndef CATEGORICAL_DISTRIBUTION_HPP_
#define CATEGORICAL_DISTRIBUTION_HPP_

#include "discrete_random_variable.hpp"

namespace cpprob
{

  class CategoricalDistribution
  {

    typedef std::map<DiscreteRandomVariable, float> ProbabilityTable;

  public:

    // Types following the map conventions
    typedef ProbabilityTable::iterator iterator;
    typedef ProbabilityTable::const_iterator const_iterator;
    typedef DiscreteRandomVariable key_type;
    typedef float mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;

    // Types following the distribution conventions
    typedef mapped_type input_type;
    typedef key_type result_type;

    CategoricalDistribution()
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

    void
    normalize();

    template<class _RandomNumberGenerator>
      result_type
      operator()(_RandomNumberGenerator& rng)
      {
        input_type cum(0);
        const input_type r = rng();
        ProbabilityTable::const_iterator it = pt_.begin();
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

private:

  ProbabilityTable pt_;

  };

  std::ostream&
  operator<<(std::ostream& os, const CategoricalDistribution& d);

}

#endif /* CATEGORICAL_DISTRIBUTION_HPP_ */

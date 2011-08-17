/*
 * categorical_distribution.hpp
 *
 *  Created on: 16.05.2011
 *      Author: wbam
 */

#ifndef CATEGORICAL_DISTRIBUTION_HPP_
#define CATEGORICAL_DISTRIBUTION_HPP_

#include "bound_probability_distribution.hpp"
#include "discrete_random_variable.hpp"
#include "random_probabilities.hpp"

namespace cpprob
{

  class CategoricalDistribution : public BoundProbabilityDistribution
  {

    typedef RandomProbabilities ProbabilityTable;

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

    CategoricalDistribution();

    CategoricalDistribution(const CategoricalDistribution& cd);

    CategoricalDistribution(DiscreteRandomVariable& var);

    CategoricalDistribution(DiscreteRandomVariable& var,
        RandomProbabilities& params);

    virtual
    ~CategoricalDistribution();

    virtual float
    at_references() const;

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
    normalize()
    {
      pt_.normalize();
    }

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

  CategoricalDistribution&
  operator=(const CategoricalDistribution& cd)
  {
    if (&pt_ != &cd.pt_)
      {
        pt_ = cd.pt_;
        var_ = cd.var_;
      }
    return *this;
  }

  virtual std::ostream&
  put_out_references(std::ostream& os) const;

  void
  set(const DiscreteRandomVariable& var, float probability)
  {
    pt_.set(var, probability);
  }

protected:

  virtual std::ostream&
  put_out(std::ostream& os) const;

private:

  bool is_pt_self_allocated;
  ProbabilityTable& pt_;
  DiscreteRandomVariable* var_;

  static RandomProbabilities&
  make_parameters_from_other_cd(const CategoricalDistribution& cd);

  };

}

#endif /* CATEGORICAL_DISTRIBUTION_HPP_ */

/*
 * bound_probability_distribution.hpp
 *
 *  Created on: 12.07.2011
 *      Author: wbam
 */

#ifndef BOUND_PROBABILITY_DISTRIBUTION_HPP_
#define BOUND_PROBABILITY_DISTRIBUTION_HPP_

#include <iosfwd>

namespace cpprob
{

  class BoundProbabilityDistribution
  {

  public:

    virtual
    ~BoundProbabilityDistribution() = 0;

    virtual float
    at_references() const = 0;

    friend std::ostream&
    operator<<(std::ostream& os, const BoundProbabilityDistribution& d)
    {
      return d.put_out(os);
    }

    virtual std::ostream&
    put_out_references(std::ostream& os) const = 0;

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const = 0;

  };

  inline
  BoundProbabilityDistribution::~BoundProbabilityDistribution()
  {
  }

}

#endif /* BOUND_PROBABILITY_DISTRIBUTION_HPP_ */

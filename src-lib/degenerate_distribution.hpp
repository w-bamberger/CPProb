/*
 * degenerate_distribution.hpp
 *
 *  Created on: 18.07.2011
 *      Author: wbam
 */

#ifndef DEGENERATE_DISTRIBUTION_HPP_
#define DEGENERATE_DISTRIBUTION_HPP_

#include "bound_probability_distribution.hpp"
#include "random_variable.hpp"

namespace cpprob
{

  class DegenerateDistribution : public BoundProbabilityDistribution
  {

  public:

    DegenerateDistribution(RandomVariable& var) :
        var_reference_(var)
    {
    }

    virtual
    ~DegenerateDistribution()
    {
    }

    virtual float
    at_references() const
    {
      return 1.0;
    }

    virtual std::ostream&
    put_out_references(std::ostream& os) const
    {
      return os << var_reference_;
    }

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const
    {
      return os << "      Degenerate distribution\n";
    }

  private:

    RandomVariable& var_reference_;

  };

}

#endif /* DEGENERATE_DISTRIBUTION_HPP_ */

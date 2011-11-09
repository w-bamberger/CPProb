/*
 * RandomInteger.hpp
 *
 *  Created on: 29.09.2011
 *      Author: wbam
 */

#ifndef RANDOMINTEGER_HPP_
#define RANDOMINTEGER_HPP_

#include "DiscreteRandomVariable.hpp"

namespace cpprob
{

  /*
   *
   */
  class RandomInteger : public cpprob::DiscreteRandomVariable
  {

  public:

    RandomInteger(const std::string& name, std::size_t size, std::size_t value)
        : DiscreteRandomVariable(name, size, value)
    {
      /* This constructor allows to change the size of an existing random
       * variable type. The constructor of DiscreteRandomVariable called above
       * does not do that. So I ensure here afterwards that the size in the
       * characteristics table meets the new size given in this constructor. */
      characteristics_->second.size_ = size;
    }

    explicit
    RandomInteger(const DiscreteRandomVariable& var)
        : DiscreteRandomVariable(var)
    {
    }

    virtual
    ~RandomInteger()
    {
    }

    std::size_t
    observation() const
    {
      return value_;
    }

    RandomInteger&
    observation(std::size_t new_value)
    {
      if (new_value < characteristics_->second.size_)
        value_ = new_value;
      else
        cpprob_throw_out_of_range(
            "RandomInteger: The new value " << new_value << " is greater than the size " << characteristics_->second.size_);

      return *this;
    }

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const;

  };

} /* namespace cpprob */

#endif /* RANDOMINTEGER_HPP_ */

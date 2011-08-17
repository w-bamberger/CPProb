/*
 * discrete_random_variable.hpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef DISCRETE_RANDOM_VARIABLE_HPP_
#define DISCRETE_RANDOM_VARIABLE_HPP_

#include "random_variable.hpp"
#include "error.hpp"
#include <tr1/type_traits>
#include <iterator>
#include <map>
#include <stdexcept>

namespace vanet
{

  class DiscreteRandomVariable;
  class DiscreteJointRandomVariable;

}

namespace std
{
  namespace tr1
  {

    template<>
      struct is_integral<vanet::DiscreteRandomVariable> : public integral_constant<
          bool, true>
      {
      };

  }
}

namespace vanet
{

  class DiscreteRandomVariable : public RandomVariable, public std::iterator<
      std::bidirectional_iterator_tag, DiscreteRandomVariable>
  {

  protected:

    class Characteristics;

  public:

    class Range
    {

    public:

      typedef DiscreteRandomVariable const_iterator;
      typedef DiscreteRandomVariable iterator;

      const_iterator
      begin() const
      {
        return DiscreteRandomVariable(characteristics_, 0);
      }

      bool
      empty() const
      {
        return characteristics_ == characteristics_table_.end()
            || characteristics_->second.size_ == 0;
      }

      const_iterator
      end() const
      {
        if (characteristics_ == characteristics_table_.end())
          return DiscreteRandomVariable(characteristics_, 0);
        else
          return DiscreteRandomVariable(characteristics_,
              characteristics_->second.size_);
      }

      std::size_t
      size() const
      {
        if (characteristics_ == characteristics_table_.end())
          return 0;
        else
          return characteristics_->second.size_;
      }

    private:

      typedef std::map<std::string, Characteristics> CharacteristicsTable;
      friend class DiscreteRandomVariable;

      const CharacteristicsTable::iterator characteristics_;

      Range(const CharacteristicsTable::iterator& characteristics) :
          characteristics_(characteristics)
      {
      }

    };

    DiscreteRandomVariable() :
        characteristics_(characteristics_table_.end()), value_(0)
    {
    }

    virtual
    ~DiscreteRandomVariable()
    {
    }

    void
    assign_random_value(RandomNumberEngine& rng);

    virtual const std::string&
    name() const
    {
      if (characteristics_ != characteristics_table_.end())
        return characteristics_->first;
      else
        return empty_string_;
    }

    virtual DiscreteRandomVariable&
    operator++()
    {
      vanet_check_debug(characteristics_ != characteristics_table_.end(),
          "DiscreteRandomVariable: Cannot decrement an empty random variable.");
      vanet_check_debug( value_ < characteristics_->second.size_,
          "DiscreteRandomVariable: Cannot increment the value " << value_ //
          << " of the variable " << name() << " above the maximum "//
          << characteristics_->second.size_ << ".");

      ++value_;
      return *this;
    }

    virtual DiscreteRandomVariable&
    operator++(int)
    {
      vanet_check_debug(characteristics_ != characteristics_table_.end(),
          "DiscreteRandomVariable: Cannot increment an empty random variable.");
      vanet_check_debug( value_ < characteristics_->second.size_,
          "DiscreteRandomVariable: Cannot increment the value " << value_ //
          << " of the variable " << name() << " above the maximum "//
          << characteristics_->second.size_ << ".");

      ++value_;
      return *this;
    }

    DiscreteRandomVariable&
    operator--()
    {
      vanet_check_debug(characteristics_ != characteristics_table_.end(),
          "DiscreteRandomVariable: Cannot decrement an empty random variable.");
      vanet_check_debug( value_ < characteristics_->second.size_,
          "DiscreteRandomVariable: Cannot decrement the value " << value_ //
          << " of the variable " << name() << " below 0.");

      --value_;
      return *this;
    }

    DiscreteRandomVariable&
    operator--(int)
    {
      vanet_check_debug(characteristics_ != characteristics_table_.end(),
          "DiscreteRandomVariable: Cannot decrement an empty random variable.");
      vanet_check_debug( value_ < characteristics_->second.size_,
          "DiscreteRandomVariable: Cannot decrement the value " << value_ //
          << " of the variable " << name() << " below 0.");

      --value_;
      return *this;
    }

    bool
    operator==(const DiscreteRandomVariable& i) const
    {
      if (characteristics_ == characteristics_table_.end()
          || i.characteristics_ == characteristics_table_.end())
        return characteristics_ == i.characteristics_;
      else
        return value_ == i.value_ && characteristics_ == i.characteristics_;
    }

    bool
    operator!=(const DiscreteRandomVariable& i) const
    {
      if (characteristics_ == characteristics_table_.end()
          || i.characteristics_ == characteristics_table_.end())
        return characteristics_ != i.characteristics_;
      else
        return value_ != i.value_ || characteristics_ != i.characteristics_;
    }

    virtual bool
    operator<(const DiscreteRandomVariable& var) const;

    virtual DiscreteRandomVariable&
    operator=(const DiscreteRandomVariable& var);

    virtual Range
    value_range() const
    {
      return Range(characteristics_);
    }

  protected:

    friend class DiscreteJointRandomVariable;

    struct Characteristics
    {
      Characteristics(std::size_t size) :
          size_(size)
      {
      }

      std::size_t size_;
    };
    typedef std::map<std::string, Characteristics> CharacteristicsTable;

    static CharacteristicsTable characteristics_table_;
    CharacteristicsTable::iterator characteristics_;
    std::size_t value_;

    DiscreteRandomVariable(
        const CharacteristicsTable::iterator& characteristics,
        std::size_t value) :
        characteristics_(characteristics), value_(value)
    {
    }

    virtual std::ostream&
    put_out(std::ostream& os) const;

    virtual std::ostream&
    put_out_characteristics_table(std::ostream& os) const;

  private:

    const static std::string empty_string_;

  };

}

#endif /* DISCRETE_RANDOM_VARIABLE_HPP_ */

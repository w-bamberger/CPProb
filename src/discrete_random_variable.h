/*
 * discrete_random_variable.h
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef DISCRETE_RANDOM_VARIABLE_H_
#define DISCRETE_RANDOM_VARIABLE_H_

#include "random_variable.h"
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
        return characteristics_->second.size_ == 0;
      }

      const_iterator
      end() const
      {
        return DiscreteRandomVariable(characteristics_,
            characteristics_->second.size_);
      }

      std::size_t
      size() const
      {
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

    DiscreteRandomVariable&
    operator++()
    {
      if (value_ < characteristics_->second.size_)
        {
          ++value_;
          return *this;
        }
      else
        {
          throw std::out_of_range(
              "RandomVariable: Cannot increase the value above the maximum.");
        }
    }

    DiscreteRandomVariable&
    operator++(int)
    {
      if (value_ < characteristics_->second.size_)
        {
          ++value_;
          return *this;
        }
      else
        {
          throw std::out_of_range(
              "RandomVariable: Cannot increase the value above the maximum.");
        }
    }

    DiscreteRandomVariable&
    operator--()
    {
      if (value_ > 0)
        {
          --value_;
          return *this;
        }
      else
        {
          throw std::out_of_range(
              "RandomVariable: Cannot decrease the value below 0.");
        }
    }

    DiscreteRandomVariable&
    operator--(int)
    {
      if (value_ > 0)
        {
          --value_;
          return *this;
        }
      else
        {
          throw std::out_of_range(
              "RandomVariable: Cannot decrease the value below 0.");
        }
    }

    bool
    operator==(const DiscreteRandomVariable& i) const
    {
      return value_ == i.value_ && characteristics_ == i.characteristics_;
    }

    bool
    operator!=(const DiscreteRandomVariable& i) const
    {
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

#endif /* DISCRETE_RANDOM_VARIABLE_H_ */

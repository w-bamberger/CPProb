/*
 * DiscreteRandomVariable.hpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef DISCRETERANDOMVARIABLE_HPP_
#define DISCRETERANDOMVARIABLE_HPP_

#include "RandomVariable.hpp"
#include "Error.hpp"
#include <iterator>
#include <stdexcept>
#include <type_traits>

#ifdef VANET_DEBUG_MODE
#include <debug/map>
namespace cpprob
{
  namespace cont
  {
    using __gnu_debug::map;
  }
}
#else
#include <map>
namespace cpprob
{
  namespace cont
  {
    using std::map;
  }
}
#endif

namespace cpprob
{

  /**
   * Represents the mathematical object of a discrete random variable.
   * This is the base class for all kinds of discrete random variables. It
   * implements the abstract concept of a discrete random variable in
   * mathematics (see below) and leaves the concrete value set to the
   * sub-classes. The common interface of all sub-classes contains the following
   * properties and operations:
   *
   * @li A name identifies the sample space (the set of outcomes) the discrete
   *     random variable is associated with. So all discrete random variables
   *     that refer to the same sample space (the same events), provide the
   *     same name.
   * @li A range to enumerate the sample space.
   * @li Enumeration operators (++ and --) and equality operations (== and !=)
   *     together with a range definition
   *     (see <a href="http://www.boost.org/doc/libs/release/libs/range/">Boost
   *     range</a>) to enumerate the set of possible values.
   * @li The less than operator (<) makes it possible to store
   *     %DiscreteRandomVariable objects in ordered containers (like set or
   *     map). They do not mean that one value is indeed greater than another
   *     value in a mathematical sense. (For example, the values “tail” and
   *     “head” have no numerical value and tail cannot be said to be greater
   *     than head or the other way around.)
   *
   * In mathematics, a random variable is a mapping from the probability space
   * in another measurable space. It is a mean to describe events of the
   * probability space. A discrete random variable is based on a finite sample
   * space (for example, Ω = {tail, head}). These outcomes need not be numbers.
   * So an int variable would unnecessarily restrict the possible values
   * (not the number of values but the semantics of the values). The class
   * %DiscreteRandomVariable leaves the definition of the correct values to
   * sub-classes. They can use booleans or strings or any other C++ type to
   * represent the values. %DiscreteRandomVariable only cares about the fact that
   * there is finite number of possible values. (More precisely, the number of
   * possible values must be representable by std::size_t.) This leads to an
   * interface of a enumerable and named object as described in the list above.
   *
   * The set of possible values is enumerable without actually knowing the
   * values. The values need not be available as objects in memory. So they
   * can only be enumerated but not be iterated. As a consequence, the
   * interface provides a range (DiscreteRandomVariable::Range) to enumerate
   * the values but not a container of the values. You can enumerate the
   * values with an iterator-like notation. But there is no iterator you can
   * dereference, as there is no object in memory the iterator can point to.
   * Instead you enumerate the object, meaning that you increase or decrease
   * its value.
   *
   * If you want to write a sub-class, you must maintain the structure of this
   * class as you cannot override the read only methods. You can but need not
   * override all modifying methods. This way, you can maintain additional
   * structure that is necessary for your sub-class. RandomBoolean is an
   * example of a class that just adds a couple of methods but does not override
   * the pre-defined methods of this class. In constrast,
   * DiscreteJointRandomVariable maintains an additional structure that depends
   * on the current value of the variable. So it must override @em all modifying
   * methods.
   */
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

      typedef cont::map<std::string, Characteristics> CharacteristicsTable;
      friend class DiscreteRandomVariable;

      const CharacteristicsTable::iterator characteristics_;

      Range(const CharacteristicsTable::iterator& characteristics)
          : characteristics_(characteristics)
      {
      }

    };

    DiscreteRandomVariable()
        : characteristics_(characteristics_table_.end()), value_(0)
    {
    }

    virtual
    ~DiscreteRandomVariable()
    {
    }

    virtual void
    assign_random_value(RandomNumberEngine& rng);

    const std::string&
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
      cpprob_check_debug(characteristics_ != characteristics_table_.end(),
          "DiscreteRandomVariable: Cannot decrement an empty random variable.");
      cpprob_check_debug( value_ < characteristics_->second.size_,
          "DiscreteRandomVariable: Cannot increment the value " << value_ //
          << " of the variable " << name() << " above the maximum "//
          << characteristics_->second.size_ << ".");

      ++value_;
      return *this;
    }

    virtual DiscreteRandomVariable&
    operator++(int)
    {
      cpprob_check_debug(characteristics_ != characteristics_table_.end(),
          "DiscreteRandomVariable: Cannot increment an empty random variable.");
      cpprob_check_debug( value_ < characteristics_->second.size_,
          "DiscreteRandomVariable: Cannot increment the value " << value_ //
          << " of the variable " << name() << " above the maximum "//
          << characteristics_->second.size_ << ".");

      ++value_;
      return *this;
    }

    virtual DiscreteRandomVariable&
    operator--()
    {
      cpprob_check_debug(characteristics_ != characteristics_table_.end(),
          "DiscreteRandomVariable: Cannot decrement an empty random variable.");
      cpprob_check_debug( value_ < characteristics_->second.size_,
          "DiscreteRandomVariable: Cannot decrement the value " << value_ //
          << " of the variable " << name() << " below 0.");

      --value_;
      return *this;
    }

    virtual DiscreteRandomVariable&
    operator--(int)
    {
      cpprob_check_debug(characteristics_ != characteristics_table_.end(),
          "DiscreteRandomVariable: Cannot decrement an empty random variable.");
      cpprob_check_debug( value_ < characteristics_->second.size_,
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

    bool
    operator<(const DiscreteRandomVariable& var) const;

    virtual DiscreteRandomVariable&
    operator=(const DiscreteRandomVariable& var);

    Range
    value_range() const
    {
      return Range(characteristics_);
    }

  protected:

    friend class DiscreteJointRandomVariable;
    friend class DiscreteRandomReferences;

    struct Characteristics
    {
      Characteristics(std::size_t size)
          : size_(size)
      {
      }

      std::size_t size_;
    };
    typedef cont::map<std::string, Characteristics> CharacteristicsTable;

    const static std::string empty_string_;
    static CharacteristicsTable characteristics_table_;
    CharacteristicsTable::iterator characteristics_;
    std::size_t value_;

    DiscreteRandomVariable(
        const CharacteristicsTable::iterator& characteristics,
        std::size_t value)
        : characteristics_(characteristics), value_(value)
    {
    }

    virtual std::ostream&
    put_out(std::ostream& os) const;

    virtual std::ostream&
    put_out_characteristics_table(std::ostream& os) const;

  };

}

namespace std
{

    /**
     * Defines DiscreteRandomVariable as an arithmetic type.
     * std::variate_generator expects that the type of an distribution
     * sample is an arithmetic type (integral or floating). This restricts
     * the sample type to the built-in types. To let DiscreteRandomVariable
     * be the result of a sampling operation, this hack is necessary. It defines
     * DiscreteRandomVariable as a integral type (comparable to int). From its
     * meaning, DiscreteRandomVariable is indeed an integral type.
     *
     * Nonetheless, the following definition is not standard conform as I modify
     * the namespace std. But it is the only solution I know to sample complex
     * objects.
     */
    template<>
      struct is_integral<cpprob::DiscreteRandomVariable> : public integral_constant<
          bool, true>
      {
      };

}

#endif /* DISCRETERANDOMVARIABLE_HPP_ */

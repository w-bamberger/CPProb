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
#include "cont/map.hpp"
#include <iterator>
#include <stdexcept>
#include <type_traits>

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
   *     with the same name refer to the same set of outcomes.
   * @li A range to enumerate the set of outcomes.
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
   * there is a finite number of possible values. (More precisely, the number of
   * possible values must be representable by std::size_t.) This leads to an
   * interface of an enumerable and named object as described in the list above.
   *
   * The set of possible outcomes is enumerable without actually knowing the
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
   * structures that are necessary for your sub-class. RandomBoolean is an
   * example of a class that just adds a couple of methods but does not override
   * the pre-defined methods of this class. In contrast,
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

    class NameLess
    {

    public:

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the first argument of operator().
       */
      typedef DiscreteRandomVariable first_argument_type;

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the second argument of operator().
       */
      typedef DiscreteRandomVariable second_argument_type;

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the result of operator().
       */
      typedef bool result_type;

      bool
      operator()(const DiscreteRandomVariable& var1,
          const DiscreteRandomVariable& var2) const
      {
        cpprob_check_debug(
            var1.characteristics_ != DiscreteRandomVariable::characteristics_table_.end() && var2.characteristics_ != DiscreteRandomVariable::characteristics_table_.end(),
            "DiscreteRandomVariable: Cannot compare a variable that is not associated with a set of observations.");
        return var1.characteristics_->first < var2.characteristics_->first;
      }

    };

    class NameValueLess
    {

    public:

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the first argument of operator().
       */
      typedef DiscreteRandomVariable first_argument_type;

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the second argument of operator().
       */
      typedef DiscreteRandomVariable second_argument_type;

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the result of operator().
       */
      typedef bool result_type;

      /**
       * This comparison works for variables with the same or different sets
       * of outcomes. One or both sets may even be empty.
       */
      bool
      operator()(const DiscreteRandomVariable& var1,
          const DiscreteRandomVariable& var2) const
      {
        /* The default value of characteristics, i.e. characteristics_table_.end(),
         * is taken as the highest value. So the ordering of the random variable
         * store objects is in order of the iterators of CharacteristicsTable.
         * The sub-ordering criterion, in case both iterators are the same,
         * is the value_. */
        if (var1.characteristics_ == characteristics_table_.end())
          /* No element can be larger than characteristics.
           * (value_ cannot be set.) */
          return false;

        else if (var2.characteristics_ == characteristics_table_.end())
          /* characteristics must be different and thus smaller
           * than var.characteristics. */
          return true;

        else if (var1.characteristics_ == var2.characteristics_)
          return var1.value_ < var2.value_;

        else
          return var1.characteristics_->first < var2.characteristics_->first;
      }

    };

    class ValueLess
    {

    public:

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the first argument of operator().
       */
      typedef DiscreteRandomVariable first_argument_type;

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the second argument of operator().
       */
      typedef DiscreteRandomVariable second_argument_type;

      /**
       * Typedef for the concept of a binary function object. It gives the
       * type of the result of operator().
       */
      typedef bool result_type;

      bool
      operator()(const DiscreteRandomVariable& var1,
          const DiscreteRandomVariable& var2) const
      {
        cpprob_check_debug(
            var1.characteristics_ != DiscreteRandomVariable::characteristics_table_.end() && var2.characteristics_ != DiscreteRandomVariable::characteristics_table_.end(),
            "DiscreteRandomVariable: Cannot compare a variable that is not associated with a set of observations.");
        cpprob_check_debug(
            var1.characteristics_->second.size_ != 0 && var1.characteristics_->second.size_ != 0,
            "DiscreteRandomVariable: Cannot compare a variable with an empty set of observations.");
        return var1.value_ < var2.value_;
      }

    };

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

    /**
     * Construct a variable not associated with a set of observations yet. You
     * cannot do anything with such a variable but assign a value from another
     * discrete random variable. So this constructor is mainly useful for
     * containers that need default construction and to define a variable in
     * a higher scope as the value assignment (e.g., in front of a loop). This
     * is the only public constructor as a discrete random variable should
     * be instantiated by its subclasses.
     *
     * So a variable without a set of observations has no value, no name, and
     * cannot be incremented or decremented. The value range of such a
     * variable is empty.
     *
     * @par Ensures:
     * - <tt>this->%name() == string()</tt>.
     * - <tt>this->%value_range().size() == 0</tt>.
     */
    DiscreteRandomVariable()
        : characteristics_(characteristics_table_.end()), value_(0)
    {
    }

    DiscreteRandomVariable(const DiscreteRandomVariable& other)
        : characteristics_(other.characteristics_), value_(other.value_)
    {
    }

    DiscreteRandomVariable(DiscreteRandomVariable&& other)
    : characteristics_(other.characteristics_), value_(other.value_)
    {
    }

    /**
     * Cleans up.
     */
    virtual
    ~DiscreteRandomVariable()
    {
    }

    // Documented in the base class.
        virtual void
        assign_random_value(RandomNumberEngine& rng);

        // Documented in the base class.
        const std::string&
        name() const
        {
          if (characteristics_ != characteristics_table_.end())
          return characteristics_->first;
          else
          return empty_string_;
        }

        /**
         * Decrements the value of this variable. This method walks in descending
         * order through the set of outcomes. It provides a reference to @c *this.
         *
         * @par Requires:
         * - This variable must be associated with a set of outcomes.
         * - The value of this variable may not be the first element in the ordered
         *   set of outcomes.
         *
         * @par Ensures:
         * - <tt>%DiscreteRandomVariable y = --x</tt> is the same as
         *   <tt>x--; %DiscreteRandomVariable y = x;</tt>.
         * - <tt>--x; ++x</tt> leaves @c x unchanged.
         *
         * @return a reference to this changed variable
         * @throw None
         * @see operator--(int)
         */
        virtual DiscreteRandomVariable&
        operator--()
        {
          cpprob_check_debug(characteristics_ != characteristics_table_.end(),
              "DiscreteRandomVariable: Cannot decrement an empty random variable.");
          cpprob_check_debug( value_ > 0,
              "DiscreteRandomVariable: Cannot decrement the value " << value_ //
              << " of the variable " << name() << " below 0.");

          --value_;
          return *this;
        }

        /**
         * Decrements the value of this variable. This method walks in descending
         * order through the set of outcomes. It provides the old value of the
         * variable.
         *
         * @par Requires:
         * - This variable must be associated with a set of outcomes.
         * - The value of this variable may not be the first element in the ordered
         *   set of outcomes.
         *
         * @par Ensures:
         * - <tt>%DiscreteRandomVariable y = x--</tt> is the same as
         *   <tt>%DiscreteRandomVariable y = x; --x</tt>.
         * - <tt>x--; x++</tt> leaves @c x unchanged.
         *
         * @return a variable with the value before the decrement
         * @throw None
         * @see #operator--()
         */
        virtual DiscreteRandomVariable
        operator--(int)
        {
          cpprob_check_debug(characteristics_ != characteristics_table_.end(),
              "DiscreteRandomVariable: Cannot decrement an empty random variable.");
          cpprob_check_debug( value_ > 0,
              "DiscreteRandomVariable: Cannot decrement the value " << value_ //
              << " of the variable " << name() << " below 0.");

          DiscreteRandomVariable tmp = *this;
          --value_;
          return tmp;
        }

        /**
         * Increments the value of this variable. This method walks in ascending
         * order through the set of outcomes. It provides a reference to @c *this.
         *
         * @par Requires:
         * - This variable must be associated with a set of outcomes.
         * - The value of this variable may not be the element behind the last
         *   element in the ordered set of outcomes.
         *
         * @par Ensures:
         * - <tt>%DiscreteRandomVariable y = ++x</tt> is the same as
         *   <tt>x++; %DiscreteRandomVariable y = x;</tt>.
         * - <tt>++x; --x</tt> leaves @c x unchanged.
         *
         * @return a reference to this changed variable
         * @throw None
         * @see operator++(int)
         */
        virtual DiscreteRandomVariable&
        operator++()
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

        /**
         * Increments the value of this variable. This method walks in ascending
         * order through the set of outcomes. It provides the old value of the
         * variable.
         *
         * @par Requires:
         * - This variable must be associated with a set of outcomes.
         * - The value of this variable may not be the element behind the last
         *   element in the ordered set of outcomes.
         *
         * @par Ensures:
         * - <tt>%DiscreteRandomVariable y = x++</tt> is the same as
         *   <tt>%DiscreteRandomVariable y = x; ++x</tt>.
         * - <tt>x++; x--</tt> leaves @c x unchanged.
         *
         * @return a variable with the value before the increment
         * @throw None
         * @see operator++()
         */
        virtual DiscreteRandomVariable
        operator++(int)
        {
          cpprob_check_debug(characteristics_ != characteristics_table_.end(),
              "DiscreteRandomVariable: Cannot increment an empty random variable.");
          cpprob_check_debug( value_ < characteristics_->second.size_,
              "DiscreteRandomVariable: Cannot increment the value " << value_ //
              << " of the variable " << name() << " above the maximum "//
              << characteristics_->second.size_ << ".");

          DiscreteRandomVariable tmp = *this;
          ++value_;
          return tmp;
        }

        /**
         * Compares two discrete random variables for equality. @c *this and @c i
         * are equal if they are associated with the same name and have the same
         * value. Two variables that are both not associated with a set of outcomes
         * are equal as well.
         *
         * @par Ensures:
         * - After the assignment <tt>*this = i</tt>, the comparison
         *   <tt>*this == i</tt> is true.
         *
         * @param i the variable to compare with
         * @return true if @c *this and @c i are equal; otherwise false
         * @throw None
         */
        bool
        operator==(const DiscreteRandomVariable& i) const
        {
          if (characteristics_ == characteristics_table_.end()
              || i.characteristics_ == characteristics_table_.end())
          return characteristics_ == i.characteristics_;
          else
          return value_ == i.value_ && characteristics_ == i.characteristics_;
        }

        /**
         * Compares two discrete random variables for inequality.
         *
         * @par Ensures:
         * - <tt>this->operator!=(i) == (! this->operator==(i))</tt>
         *
         * @param i the variable to compare with
         * @return true if @c i and @c *this are different; otherwise false
         * @throw None
         * @see #operator==
         */
        bool
        operator!=(const DiscreteRandomVariable& i) const
        {
          if (characteristics_ == characteristics_table_.end()
              || i.characteristics_ == characteristics_table_.end())
          return characteristics_ != i.characteristics_;
          else
          return value_ != i.value_ || characteristics_ != i.characteristics_;
        }

        /**
         * Determines if this variable is lower than the given variable @c var
         * following the implicit ordering of the set of outcomes.
         *
         * @par Requries:
         * - Both variables must belong to the same set of outcomes
         *   (<tt>%name() == var.%name()</tt>). (Only checked in debug mode.)
         *
         * @par Ensures:
         * - <tt>(*this < var) == (var > *this)</tt>.
         * - Only one of the operations @c >, @c < and @c == is true.
         *
         * @param var the variable to compare with
         * @return true if this variable is lower than @c var; otherwise false.
         */
        bool
        operator<(const DiscreteRandomVariable& var) const
        {
          cpprob_check_debug(
              characteristics_ == var.characteristics_,
              "DiscreteRandomVariable: Can only compare outcomes of the same kind of random variable, but found the two types " << characteristics_->first << " and " << var.characteristics_->first);
          return value_ < var.value_;
        }

        /**
         * Determines if this variable is greater than the given variable @c var
         * following the implicit ordering of the set of outcomes.
         *
         * @par Requries:
         * - Both variables must belong to the same set of outcomes
         *   (<tt>%name() == var.%name()</tt>). (Only checked in debug mode.)
         *
         * @par Ensures:
         * - <tt>(*this > var) == (var < *this)</tt>.
         * - Only one of the operations @c >, @c < and @c == is true.
         *
         * @param var the variable to compare with
         * @return true if this variable is greater than @c var; otherwise false.
         */
        bool
        operator>(const DiscreteRandomVariable& var) const
        {
          cpprob_check_debug(
              characteristics_ == var.characteristics_,
              "DiscreteRandomVariable: Can only compare outcomes of the same kind of random variable.");
          return value_ > var.value_;
        }

        /**
         * Assigns the value of another variable to this variable. Only variables
         * of the same kind (i.e., with the same entry in the characteristics
         * table) may be assigned to each other. The only exception is variable that
         * is not associated with set of outcomes (the variable is default
         * constructed). Such a variable may receive the value of any other
         * discrete random variable. And in this case, it also inherits the set of
         * outcome of @c var.
         *
         * @par Requires:
         * - This variable is not associated with a set of outcomes or both
         *   sets are the same (<tt>var.name() == this->%name()</tt>). This
         *   requirement is only checked in debug mode.
         * - @c var must be associated with a set of outcomes. This
         *   requirement is only checked in debug mode.
         *
         * @par Ensures:
         * - <tt>*this == var</tt>
         *
         * @param var the variable whose value is assigned to this variable
         * @return a reference to @c *this
         * @throw None
         */
        virtual DiscreteRandomVariable&
        operator=(const DiscreteRandomVariable& var);

        virtual DiscreteRandomVariable&
        operator=(DiscreteRandomVariable&& var);

        /**
         * Puts out the value of a random variable in the form @em name:value.
         * The exact form depends on the value. For example, random vectors need
         * a more extended syntax for their output.
         *
         * This method allow polymorphic output streaming. So every random
         * variable can be streamed out without knowing its exact type.
         *
         * @param os the output stream to use
         * @param r the random variable the value of which is printed out
         * @return the output stream given as the first argument
         */
        friend std::ostream&
        operator<<(std::ostream& os, const DiscreteRandomVariable& r)
        {
          return r.put_out(os);
        }

        /**
         * Provides a range object that allows to enumerate all outcomes of this
         * variable. If the variable is not associated with a set of outcomes,
         * the provided range is empty. (An associated variable may also
         * have an empty range if the set of outcomes is empty.)
         *
         * @return the range of this random variable
         * @throw None
         */
        Range
        value_range() const
        {
          return Range(characteristics_);
        }

      protected:

        /**
         * Type of an entry in the characteristics table.
         *
         * @see #CharacteristicsTable
         */
        struct Characteristics
        {
          Characteristics(std::size_t size)
          : size_(size)
          {
          }

          std::size_t size_;
        };

        /**
         * Type of the table that stores the names and sizes of all discrete
         * random variables. The name of a random variable might be something like
         * @em FirstDie, @em SecondDice, @em Flavor, @em Wrapper etc. They represent
         * what is measured from an experiment's outcome. These names are mostly
         * used for to generate understandable output.
         *
         * In addition, the name of a random variable refers to a set of possible
         * outcome values. The variable can only take on values of this set. For
         * example, if one variable with the name GreenWrapper is a boolean variable
         * (set of outcomes Ω = {true, false}), then another variable with the
         * same name refers to the same set of outcomes. But there may be several
         * more variables with the same set of outcomes (boolean) but different
         * names. The set of outcomes is described here by the size of the index
         * set.
         *
         */
        typedef cont::map<std::string, Characteristics> CharacteristicsTable;

        /**
         * String that is used as the name of an uninitialized variable. Since
         * the method #name returns a reference to a string, there must be a string
         * in the memory for uninitialized variables.
         */
        const static std::string empty_string_;

        /**
         * Table that stores the names and sizes of all discrete random variables.
         *
         * @see #CharacteristicsTable
         */
        static CharacteristicsTable characteristics_table_;

        /**
         * Iterator to the entry in the characteristics table that describes this
         * discrete random variable. Through this iterator, the variable knows its
         * name and its size.
         *
         * @see #CharacteristicsTable
         */
        CharacteristicsTable::iterator characteristics_;

        /**
         * Index that represents the value of this discrete random variable. To
         * implement discrete random variables in a form independent of the type
         * of outcome, each elementary outcome in the sample space is associated
         * with an index. This index is stored in @c value_ instead of the outcome.
         */
        std::size_t value_;

        /**
         * Creates a new random variable associated with a certain entry in the
         * characteristics table. This constructor gives the chance to specify
         * the name and size of the random variable directly by the pointer to the
         * entry in the characteristics table. So this constructor avoid a search
         * in the characteristics table.
         *
         * @param characteristics an iterator to the name and size in the
         *             characteristics table
         * @param value the index that represent the outcome stored in this variable
         * @throw None
         */
        DiscreteRandomVariable(
            const CharacteristicsTable::iterator& characteristics,
            std::size_t value)
        : characteristics_(characteristics), value_(value)
        {
        }

        /**
         * Creates a new random variable from the given data of the characteristics
         * table and from the value. If an entry in the characteristics table
         * exists for the given name, the new discrete random variable uses it.
         * Otherwise a new entry is created with the given name and the given size.
         *
         * @param name the name associated with the random variable
         * @param size the number of elements in the sample space
         * @param value the index that represent the outcome stored in this variable
         * @throw None
         * @see #set_up_characteristics
         */
        DiscreteRandomVariable(const std::string& name, std::size_t size,
            std::size_t value)
        : characteristics_(set_up_characteristics(name, size)), value_(value)
        {
        }

        // Documented in the base class.
        virtual std::ostream&
        put_out(std::ostream& os) const;

        /**
         * Prints the characteristics table to the given stream. This is for
         * debugging purposes.
         *
         * @param os the stream to which the characteristics table should be written
         * @return the stream provided by the parameter @c os
         * @throw ios_base::failure Streaming errors may be reported as exceptions
         *            if the stream is configure to do so
         */
        virtual std::ostream&
        put_out_characteristics_table(std::ostream& os) const;

        /**
         * Configures the characteristics data structure and iterator. This method
         * looks for an entry in the characteristics table with the same name as
         * the given name. If this entry is found, #characteristics_ is set to
         * point to this entry. If no entry with the given name is found, a new
         * entry is added to the characteristics table. The new entry has the
         * given name and the given size property.
         *
         * Note: If an entry with the given name already exists in the
         * characteristics table, the @c size parameter is ignored.
         *
         * @param name the name of this random random variable
         * @param size the size of the value range of this random variable
         * @throw None
         */
        static CharacteristicsTable::iterator
        set_up_characteristics(const std::string& name, std::size_t size);

      private:

        friend class DiscreteJointRandomVariable;
        friend class DiscreteRandomReferences;
        template<class T>
        friend class DiscreteRandomVariableMap;
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

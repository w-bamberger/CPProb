/*
 * DiscreteJointRandomVariabe.hpp
 *
 *  Created on: 24.05.2011
 *      Author: wbam
 */

#ifndef DISCRETEJOINTRANDOMVARIABLE_HPP_
#define DISCRETEJOINTRANDOMVARIABLE_HPP_

#include "DiscreteRandomVariable.hpp"
#include <set>

namespace cpprob
{

  class DiscreteJointRandomVariable : public DiscreteRandomVariable
  {

    typedef std::set<DiscreteRandomVariable, DiscreteRandomVariable::NameLess> Variables;

  public:

    typedef Variables::const_iterator const_iterator;
    typedef Variables::iterator iterator;

    DiscreteJointRandomVariable();

    /**
     * @par Requires:
     * - @c var1 and @c var2 must be associated with a set of outcomes.
     *   (They may not be default constructed without further assignment.)
     * - @c var1 and @c var2 must have a valid value. (var1.value_range().end()
     *   and var2.value_range().end() are out of the valid value range and
     *   thus invalid.)
     * - <tt>var1.name() != var2.name()</tt>
     *
     * @throw std::invalid_argument if one of the requirements fails
     */
    DiscreteJointRandomVariable(const DiscreteRandomVariable& var1,
        const DiscreteRandomVariable& var2);

    virtual
    ~DiscreteJointRandomVariable();

    const_iterator
    begin() const
    {
      return variables_.begin();
    }

    iterator
    begin()
    {
      return variables_.begin();
    }

    bool
    contains(const DiscreteRandomVariable& var)
    {
      return variables_.find(var) != variables_.end();
    }

    const_iterator
    end() const
    {
      return variables_.end();
    }

    iterator
    end()
    {
      return variables_.end();
    }

    const_iterator
    find(const std::string& name) const;

    iterator
    find(const std::string& name);

    std::pair<iterator, bool>
    insert(const DiscreteRandomVariable& var);

    virtual DiscreteJointRandomVariable&
    operator--();

    virtual DiscreteRandomVariable
    operator--(int);

    virtual DiscreteJointRandomVariable&
    operator++();

    virtual DiscreteRandomVariable
    operator++(int);

    virtual DiscreteJointRandomVariable&
    operator=(const DiscreteJointRandomVariable& v);

    std::size_t
    size() const
    {
      return variables_.size();
    }

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const;

  private:

    Variables variables_;

    /**
     * Block the usage of this operator. It is invalid.
     */
    virtual DiscreteRandomVariable&
    operator=(const DiscreteRandomVariable& v)
    {
      return *this;
    }

    void
    update_random_variable();

  };

}

#endif /* DISCRETE_JOINT_RANDOM_VARIABLE_HPP_ */

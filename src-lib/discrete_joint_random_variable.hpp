/*
 * joint_random_variable.hpp
 *
 *  Created on: 24.05.2011
 *      Author: wbam
 */

#ifndef DISCRETE_JOINT_RANDOM_VARIABLE_HPP_
#define DISCRETE_JOINT_RANDOM_VARIABLE_HPP_

#include "discrete_random_variable.hpp"
#include <set>

namespace cpprob
{

  class DiscreteJointRandomVariable : public DiscreteRandomVariable
  {

  public:

    DiscreteJointRandomVariable();

    DiscreteJointRandomVariable(const DiscreteRandomVariable& var1,
        const DiscreteRandomVariable& var2);

    virtual
    ~DiscreteJointRandomVariable();

    bool
    contains(const DiscreteRandomVariable& var)
    {
      return variables_.find(var) != variables_.end();
    }

    void
    insert(const DiscreteRandomVariable& var);

    virtual DiscreteRandomVariable&
    operator++();

    virtual DiscreteRandomVariable&
    operator++(int);

    /// @todo Some more operators are missing here. The default implementation
    /// of RandomVariable is not complete for JointRandomVariable.

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const;

  private:

    typedef std::set<DiscreteRandomVariable> Variables;

    Variables variables_;

    void
    set_up_characteristics(const std::string& name, std::size_t size);

    void
    update_random_variable();

  };

}

#endif /* DISCRETE_JOINT_RANDOM_VARIABLE_HPP_ */

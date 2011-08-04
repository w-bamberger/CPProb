/*
 * joint_random_variable.h
 *
 *  Created on: 24.05.2011
 *      Author: wbam
 */

#ifndef DISCRETE_JOINT_RANDOM_VARIABLE_H_
#define DISCRETE_JOINT_RANDOM_VARIABLE_H_

#include "discrete_random_variable.h"
#include <set>

namespace vanet
{

  class DiscreteJointRandomVariable : public DiscreteRandomVariable
  {

  public:

    DiscreteJointRandomVariable();

    DiscreteJointRandomVariable(const DiscreteRandomVariable& var1,
        const DiscreteRandomVariable& var2);

    virtual
    ~DiscreteJointRandomVariable();

    virtual bool
    contains(const DiscreteRandomVariable& var)
    {
      return variables_.find(var) != variables_.end();
    }

    virtual void
    insert(const DiscreteRandomVariable& var);

    DiscreteRandomVariable&
    operator++();

    DiscreteRandomVariable&
    operator++(int);

    /// @todo Some more operators are missing here. The default implementation
    /// of RandomVariable is not complete for JointRandomVariable.

  protected:

    virtual std::ostream&
    put_out(std::ostream& os) const;

  private:

    std::set<DiscreteRandomVariable> variables_;

    void
    set_up_characteristics(const std::string& name, std::size_t size);

    void
    update_random_variable();

  };

}

#endif /* DISCRETE_JOINT_RANDOM_VARIABLE_H_ */

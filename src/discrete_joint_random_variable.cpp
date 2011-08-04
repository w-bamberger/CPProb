/*
 * joint_random_variable.cpp
 *
 *  Created on: 27.05.2011
 *      Author: wbam
 */

#include "discrete_joint_random_variable.h"
#include "io_utils.h"
#include <tr1/tuple>
#include <algorithm>

using namespace std;
using namespace std::tr1;

namespace vanet
{

  DiscreteJointRandomVariable::DiscreteJointRandomVariable()
  {
    set_up_characteristics("EmptyJointRandomVariable", 0);
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable::DiscreteJointRandomVariable(
      const DiscreteRandomVariable& var1, const DiscreteRandomVariable& var2)
  {
    set_up_characteristics("EmptyJointRandomVariable", 0);
    insert(var1);
    insert(var2);
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable::~DiscreteJointRandomVariable()
  {
  }

  //----------------------------------------------------------------------------

  void
  DiscreteJointRandomVariable::insert(const DiscreteRandomVariable& var)
  {
    if (!variables_.insert(var).second)
      throw runtime_error(
          "JointRandomVariable: Could not insert variable " + var.name() + ".");

    update_random_variable();
  }

  //----------------------------------------------------------------------------

  DiscreteRandomVariable&
  DiscreteJointRandomVariable::operator++()
  {
    for (set<DiscreteRandomVariable>::reverse_iterator it = variables_.rbegin();
        it != variables_.rend(); ++it)
          {
      if (*it != it->value_range().end())
        {
          DiscreteRandomVariable& var = const_cast<DiscreteRandomVariable&>(*it);
          ++var;
        }
      else
        {
          DiscreteRandomVariable& var = const_cast<DiscreteRandomVariable&>(*it);
          var = it->value_range().begin();
        }
    }

  update_random_variable();
  return *this;
}

//----------------------------------------------------------------------------

DiscreteRandomVariable&
DiscreteJointRandomVariable::operator++(int)
{
  for (set<DiscreteRandomVariable>::reverse_iterator it = variables_.rbegin();
      it != variables_.rend(); ++it)
        {
    if (*it != it->value_range().end())
      {
        DiscreteRandomVariable& var = const_cast<DiscreteRandomVariable&>(*it);
        ++var;
      }
    else
      {
        DiscreteRandomVariable& var = const_cast<DiscreteRandomVariable&>(*it);
        var = it->value_range().begin();
      }
  }

update_random_variable();
return *this;
}

 //----------------------------------------------------------------------------

ostream&
DiscreteJointRandomVariable::put_out(ostream& os) const
{
DiscreteRandomVariable::put_out(os);
os << " (";
for_each(variables_.begin(), variables_.end(), StreamOut(os, " ", " "));
return os << ")";
}

 //----------------------------------------------------------------------------

void
DiscreteJointRandomVariable::set_up_characteristics(const string& name,
  size_t size)
{
characteristics_ = characteristics_table_.find(name);
if (characteristics_ == characteristics_table_.end())
  {
    bool success;
    tie(characteristics_, success) = characteristics_table_.insert(
        make_pair(name, Characteristics(size)));
    if (!success)
      throw logic_error(
          "JointRandomVariable: Could not register random variable " + name
              + ".");
  }
}

 //----------------------------------------------------------------------------

void
DiscreteJointRandomVariable::update_random_variable()
{
string new_name;
const string& old_name = characteristics_->first;
size_t new_size = 1;
value_ = 0;

for (set<DiscreteRandomVariable>::const_iterator it = variables_.begin();
    it != variables_.end(); ++it)
      {
  new_name += it->name();

  value_ += new_size * it->value_;
  new_size *= it->characteristics_->second.size_;
}

if (new_name != old_name)
set_up_characteristics(new_name, new_size);
else
characteristics_->second.size_ = new_size;
}

}

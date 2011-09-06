/*
 * DiscreteJointRandomVariable.cpp
 *
 *  Created on: 27.05.2011
 *      Author: wbam
 */

#include "DiscreteJointRandomVariable.hpp"
#include "IoUtils.hpp"
#include <tr1/tuple>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace std::tr1;

namespace cpprob
{

  DiscreteJointRandomVariable::DiscreteJointRandomVariable()
  {
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable::DiscreteJointRandomVariable(
      const DiscreteRandomVariable& var1, const DiscreteRandomVariable& var2)
  {
    if (!variables_.insert(var1).second)
      throw logic_error(
          "JointRandomVariable: Could not insert variable " + var1.name()
              + ".");
    if (!variables_.insert(var2).second)
      throw logic_error(
          "JointRandomVariable: Could not insert variable " + var2.name()
              + ".");
    update_random_variable();
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable::~DiscreteJointRandomVariable()
  {
  }

  //----------------------------------------------------------------------------

  void
  DiscreteJointRandomVariable::insert(const DiscreteRandomVariable& var)
  {
    cpprob_check_debug( var.characteristics_ != characteristics_table_.end(),
        "DiscreteJointRandomVariable: Cannot insert an empty random variable.");
    if (!variables_.insert(var).second)
      throw runtime_error(
          "JointRandomVariable: Could not insert variable " + var.name() + ".");

    update_random_variable();
  }

  //----------------------------------------------------------------------------

  DiscreteRandomVariable&
  DiscreteJointRandomVariable::operator++()
  {
    for (Variables::reverse_iterator it = variables_.rbegin();
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
    for (Variables::reverse_iterator it = variables_.rbegin();
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
    characteristics_ = characteristics_table_.lower_bound(name);
    if (characteristics_ == characteristics_table_.end()
        || characteristics_->first != name)
    {
      if (characteristics_ == characteristics_table_.begin())
        characteristics_ = characteristics_table_.insert(
            make_pair(name, Characteristics(size))).first;
      else
        characteristics_ = characteristics_table_.insert(--characteristics_,
            make_pair(name, Characteristics(size)));
    }
  }

  //----------------------------------------------------------------------------

  void
  DiscreteJointRandomVariable::update_random_variable()
  {
    string new_name;
    size_t new_size = 1;
    value_ = 0;

    for (Variables::const_iterator it = variables_.begin();
        it != variables_.end(); ++it)
        {
      cpprob_check_debug( it->characteristics_ != characteristics_table_.end(),
          "DiscreteJointRandomVariable: Cannot join an empty random variable.");

      new_name += it->name();
      value_ += new_size * it->value_;
      new_size *= it->characteristics_->second.size_;
    }

    if (characteristics_ == characteristics_table_.end()
        || new_name != characteristics_->first)
      set_up_characteristics(new_name, new_size);
    else
      characteristics_->second.size_ = new_size;
  }

}

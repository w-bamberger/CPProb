/*
 * DiscreteJointRandomVariable.cpp
 *
 *  Created on: 27.05.2011
 *      Author: wbam
 */

#include "DiscreteJointRandomVariable.hpp"
#include "IoUtils.hpp"
#include <algorithm>
#include <tuple>

using namespace std;

namespace cpprob
{

  DiscreteJointRandomVariable::DiscreteJointRandomVariable()
  {
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable::DiscreteJointRandomVariable(
      const DiscreteRandomVariable& var1, const DiscreteRandomVariable& var2)
  {
    if (var1.characteristics_ == characteristics_table_.end()
        || var1.value_ == var1.characteristics_->second.size_)
      throw invalid_argument(
          "JointRandomVariable: Could not insert variable " + var1.name()
              + ".");

    if (var2.characteristics_ == characteristics_table_.end()
        || var2.value_ == var2.characteristics_->second.size_)
      throw invalid_argument(
          "JointRandomVariable: Could not insert variable " + var2.name()
              + ".");

    if (!variables_.insert(var1).second)
      throw invalid_argument(
          "JointRandomVariable: Could not insert variable " + var1.name()
              + ".");
    if (!variables_.insert(var2).second)
      throw invalid_argument(
          "JointRandomVariable: Could not insert variable " + var2.name()
              + ".");

    update_random_variable();
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable::~DiscreteJointRandomVariable()
  {
  }

  DiscreteJointRandomVariable::const_iterator
  DiscreteJointRandomVariable::find(const std::string& name) const
  {
    /// @todo Takes no advantage of the sorting of the set.
    const_iterator search = begin();
    for (; search != end(); ++search)
    {
      if (search->name() == name)
        return search;
    }
    return search;
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable::iterator
  DiscreteJointRandomVariable::find(const std::string& name)
  {
    /// @todo Takes no advantage of the sorting of the set.
    iterator search = begin();
    for (; search != end(); ++search)
    {
      if (search->name() == name)
        return search;
    }
    return search;
  }

  //----------------------------------------------------------------------------

  pair<DiscreteJointRandomVariable::iterator, bool>
  DiscreteJointRandomVariable::insert(const DiscreteRandomVariable& var)
  {
    if (var.characteristics_ == characteristics_table_.end()
        || var.value_ == var.characteristics_->second.size_)
      return make_pair(variables_.end(), false);

    pair<iterator, bool> result = variables_.insert(var);
    if (result.second == false)
      return result;

    update_random_variable();
    return result;
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable&
  DiscreteJointRandomVariable::operator--()
  {
    cpprob_check_debug(
        characteristics_ != characteristics_table_.end(),
        "DiscreteJointRandomVariable: Cannot decrement an empty random variable.");
    cpprob_check_debug( value_ > 0,
        "DiscreteJointRandomVariable: Cannot decrement the value " << value_ //
        << " of the variable " << name() << " below 0.");

    for (Variables::iterator it = variables_.begin(); it != variables_.end();
        ++it)
    {
      DiscreteRandomVariable& var = const_cast<DiscreteRandomVariable&>(*it);
      if (var == var.value_range().begin())
      {
        var = var.value_range().end();
        --var;
      }
      else
      {
        --var;
        break;
      }
    }

    update_random_variable();
    return *this;
  }

  //----------------------------------------------------------------------------

  DiscreteRandomVariable
  DiscreteJointRandomVariable::operator--(int)
  {
    cpprob_check_debug(
        characteristics_ != characteristics_table_.end(),
        "DiscreteJointRandomVariable: Cannot decrement an empty random variable.");
    cpprob_check_debug( value_ > 0,
        "DiscreteJointRandomVariable: Cannot decrement the value " << value_ //
        << " of the variable " << name() << " below 0.");

    DiscreteRandomVariable tmp = *this;

    for (Variables::iterator it = variables_.begin(); it != variables_.end();
        ++it)
    {
      DiscreteRandomVariable& var = const_cast<DiscreteRandomVariable&>(*it);
      if (var == var.value_range().begin())
      {
        var = var.value_range().end();
        --var;
      }
      else
      {
        --var;
        break;
      }
    }

    update_random_variable();
    return tmp;
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable&
  DiscreteJointRandomVariable::operator++()
  {
    cpprob_check_debug(
        characteristics_ != characteristics_table_.end(),
        "DiscreteJointRandomVariable: Cannot increment an empty random variable.");
    cpprob_check_debug( value_ < characteristics_->second.size_,
        "DiscreteJointRandomVariable: Cannot increment the value " << value_ //
        << " of the variable " << name() << " above the maximum "//
        << characteristics_->second.size_ << ".");

    if (value_ < characteristics_->second.size_ - 1)
    {
      for (Variables::iterator it = variables_.begin(); it != variables_.end();
          ++it)
      {
        DiscreteRandomVariable& var = const_cast<DiscreteRandomVariable&>(*it);
        ++var;
        if (var == var.value_range().end())
          var = var.value_range().begin();
        else
          break;
      }
    }
    else if (value_ == characteristics_->second.size_ - 1)
    {
      DiscreteRandomVariable& var =
          const_cast<DiscreteRandomVariable&>(*(variables_.begin()));
      ++var;
    }

    update_random_variable();
    return *this;
  }

//----------------------------------------------------------------------------

  DiscreteRandomVariable
  DiscreteJointRandomVariable::operator++(int)
  {
    cpprob_check_debug(
        characteristics_ != characteristics_table_.end(),
        "DiscreteJointRandomVariable: Cannot increment an empty random variable.");
    cpprob_check_debug( value_ < characteristics_->second.size_,
        "DiscreteJointRandomVariable: Cannot increment the value " << value_ //
        << " of the variable " << name() << " above the maximum "//
        << characteristics_->second.size_ << ".");

    DiscreteRandomVariable tmp = *this;

    if (value_ < characteristics_->second.size_ - 1)
    {
      for (Variables::iterator it = variables_.begin(); it != variables_.end();
          ++it)
      {
        DiscreteRandomVariable& var = const_cast<DiscreteRandomVariable&>(*it);
        ++var;
        if (var == var.value_range().end())
          var = var.value_range().begin();
        else
          break;
      }
    }
    else if (value_ == characteristics_->second.size_ - 1)
    {
      DiscreteRandomVariable& var =
          const_cast<DiscreteRandomVariable&>(*(variables_.begin()));
      ++var;
    }

    update_random_variable();
    return tmp;
  }

  //----------------------------------------------------------------------------

  DiscreteJointRandomVariable&
  DiscreteJointRandomVariable::operator=(const DiscreteJointRandomVariable& v)
  {
    DiscreteRandomVariable::operator =(
        static_cast<const DiscreteRandomVariable&>(v));
    variables_ = v.variables_;
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
      characteristics_ = set_up_characteristics(new_name, new_size);
    characteristics_->second.size_ = new_size;
  }

}

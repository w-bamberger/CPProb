/*
 * DiscreteRandomReferences.cpp
 *
 *  Created on: 30.08.2011
 *      Author: wbam
 */

#include "DiscreteRandomReferences.hpp"
#include <algorithm>
#include <iterator>

using namespace std;

namespace cpprob
{

  DiscreteRandomVariable
  DiscreteRandomReferences::joint_value() const
  {
    size_t range_size = 1;
    size_t value = 0;

    for (const_iterator it = begin(); it != end(); ++it)
    {
      cpprob_check_debug(
          it->characteristics_ != DiscreteRandomVariable::characteristics_table_.end(),
          "DiscreteRandomReferences: Cannot join an empty random variable.");

      value += range_size * it->value_;
      range_size *= it->characteristics_->second.size_;
    }

    if (characteristics_
        == DiscreteRandomVariable::characteristics_table_.end())
      set_up_characteristics();

    return DiscreteRandomVariable(characteristics_, value);
  }

  //---------------------------------------------------------------------------

  void
  DiscreteRandomReferences::set_up_characteristics() const
  {
    typedef DiscreteRandomVariable::Characteristics Characteristics;
    string name;
    size_t range_size = 1;

    for (const_iterator it = begin(); it != end(); ++it)
    {
      cpprob_check_debug(
          it->characteristics_ != DiscreteRandomVariable::characteristics_table_.end(),
          "DiscreteRandomReferences: Cannot join an empty random variable.");

      name += it->name();
      range_size *= it->characteristics_->second.size_;
    }

    characteristics_ =
        DiscreteRandomVariable::characteristics_table_.lower_bound(name);
    if (characteristics_ == DiscreteRandomVariable::characteristics_table_.end()
        || characteristics_->first != name)
    {
      if (characteristics_
          == DiscreteRandomVariable::characteristics_table_.begin())
        characteristics_ =
            DiscreteRandomVariable::characteristics_table_.insert(
                make_pair(name, Characteristics(range_size))).first;
      else
        characteristics_ =
            DiscreteRandomVariable::characteristics_table_.insert(
                --characteristics_,
                make_pair(name, Characteristics(range_size)));
    }
  }

} /* namespace cpprob */

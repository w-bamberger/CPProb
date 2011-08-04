/*
 * boolean_random_variable.cpp
 *
 *  Created on: 24.05.2011
 *      Author: wbam
 */

#include "boolean_random_variable.h"
#include <tr1/tuple>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace std::tr1;

namespace vanet
{

  class BooleanRandomVariable::Initialiser
  {

  public:

    Initialiser()
    {
      observation_value_map_[false] = 0;
      text_value_map_["false"] = 0;
      observation_value_map_[true] = 1;
      text_value_map_["true"] = 1;
    }

  };

  map<bool, size_t> BooleanRandomVariable::observation_value_map_;
  map<string, size_t> BooleanRandomVariable::text_value_map_;
  BooleanRandomVariable::Initialiser BooleanRandomVariable::initialiser_;

  BooleanRandomVariable::BooleanRandomVariable(const string& name,
      bool observation)
  {
    set_up_characteristics(name);
    value_ = observation_value_map_[observation];
  }

  BooleanRandomVariable::BooleanRandomVariable(const string& name,
      const string& observation_text)
  {
    set_up_characteristics(name);
    value_ = text_value_map_[observation_text];
  }

  ostream&
  BooleanRandomVariable::put_out(ostream& os) const
  {
    return os << characteristics_->first << ":" << value_;
  }

  void
  BooleanRandomVariable::set_up_characteristics(const string& name)
  {
    characteristics_ = characteristics_table_.find(name);
    if (characteristics_ == characteristics_table_.end())
      {
        bool success;
        tie(characteristics_, success) = characteristics_table_.insert(
            make_pair(name, Characteristics(2)));
        if (!success)
          throw logic_error(
              "BooleanRandomVariable: Could not register random variable "
                  + name + ".");
      }
  }

}

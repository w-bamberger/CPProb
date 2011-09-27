/*
 * RandomBoolean.cpp
 *
 *  Created on: 24.05.2011
 *      Author: wbam
 */

#include "RandomBoolean.hpp"
#include <stdexcept>
#include <tuple>

using namespace std;

namespace cpprob
{

  class RandomBoolean::Initialiser
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

  map<bool, size_t> RandomBoolean::observation_value_map_;
  map<string, size_t> RandomBoolean::text_value_map_;
  RandomBoolean::Initialiser RandomBoolean::initialiser_;

  RandomBoolean::RandomBoolean(const string& name,
      bool observation)
  {
    set_up_characteristics(name);
    value_ = observation_value_map_[observation];
  }

  RandomBoolean::RandomBoolean(const string& name,
      const string& observation_text)
  {
    set_up_characteristics(name);
    value_ = text_value_map_[observation_text];
  }

  ostream&
  RandomBoolean::put_out(ostream& os) const
  {
    return os << characteristics_->first << ":" << value_;
  }

  void
  RandomBoolean::set_up_characteristics(const string& name)
  {
    characteristics_ = characteristics_table_.lower_bound(name);
    if (characteristics_ == characteristics_table_.end()
        || characteristics_->first != name)
    {
      if (characteristics_ == characteristics_table_.begin())
      {
        characteristics_ = characteristics_table_.insert(
            make_pair(name, Characteristics(2))).first;
      }
      else
      {
        characteristics_ = characteristics_table_.insert(--characteristics_,
            make_pair(name, Characteristics(2)));
      }
    }
  }

}

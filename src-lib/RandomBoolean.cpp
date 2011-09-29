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

  RandomBoolean::RandomBoolean(const string& name, bool observation)
      : DiscreteRandomVariable(name, 2, observation_value_map_[observation])
  {
  }

  RandomBoolean::RandomBoolean(const string& name,
      const string& observation_text)
      : DiscreteRandomVariable(name, 2, text_value_map_[observation_text])
  {
  }

  ostream&
  RandomBoolean::put_out(ostream& os) const
  {
    return os << characteristics_->first << ":" << value_;
  }

}

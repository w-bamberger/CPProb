/*
 * boolean_random_variable.hpp
 *
 *  Created on: 24.05.2011
 *      Author: wbam
 */

#ifndef BOOLEAN_RANDOM_VARIABLE_HPP_
#define BOOLEAN_RANDOM_VARIABLE_HPP_

#include "discrete_random_variable.hpp"

namespace vanet
{

  class BooleanRandomVariable : public DiscreteRandomVariable
  {

  public:

    BooleanRandomVariable(const std::string& name, bool observation);

    BooleanRandomVariable(const std::string& name,
        const std::string& observation_text);

    virtual BooleanRandomVariable&
    observation(const bool& new_observation)
    {
      value_ = observation_value_map_[new_observation];
      return *this;
    }

  protected:

    static std::map<bool, std::size_t> observation_value_map_;
    static std::map<std::string, std::size_t> text_value_map_;

    std::ostream&
    put_out(std::ostream& os) const;

    void
    set_up_characteristics(const std::string& name);

  private:

    class Initialiser;

    static Initialiser initialiser_;

  };
}

#endif /* BOOLEAN_RANDOM_VARIABLE_HPP_ */

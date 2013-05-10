/*
 * ListNumberGenerator.cpp
 *
 *  Created on: 05.05.2013
 *      Author: wbam
 */

#include "ListNumberGenerator.hpp"
#include <algorithm>
#include <stdexcept>

using namespace std;

namespace cpprob
{

  static unsigned int
  convert_canonical_to_internal(float canonical_number)
  {
    return static_cast<unsigned int>(canonical_number
        * static_cast<float>(numeric_limits<unsigned int>::max()));
  }

  ListNumberGenerator
  ListNumberGenerator::from_canonical(float canonical)
  {
    return ListNumberGenerator(convert_canonical_to_internal(canonical));
  }

  ListNumberGenerator
  ListNumberGenerator::from_canonical(initializer_list<float> canonicals)
  {
    ListNumberGenerator generator;
    generator.seed_from_canonical(canonicals);
    return generator;
  }

  unsigned int
  ListNumberGenerator::operator()()
  {
    if (numbers_.size() != 0)
    {
      auto number = numbers_.front();
      numbers_.pop_front();
      return number;
    }
    else
    {
      throw runtime_error("ListNumberGenerator: A number is requested, "
          "but the list of numbers is already empty.");
    }
  }

  void
  ListNumberGenerator::seed_from_canonical(float canonical)
  {
    seed(convert_canonical_to_internal(canonical));
  }

  void
  ListNumberGenerator::seed_from_canonical(
      std::initializer_list<float> new_canonicals)
  {
    numbers_.clear();
    auto insert_begin = inserter(numbers_, numbers_.begin());
    transform(new_canonicals.begin(), new_canonicals.end(), insert_begin,
        &convert_canonical_to_internal);
  }

} /* namespace cpprob */

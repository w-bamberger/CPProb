/*
 * RandomInteger.cpp
 *
 *  Created on: 29.09.2011
 *      Author: wbam
 */

#include "RandomInteger.hpp"

using namespace std;

namespace cpprob
{

  ostream&
  RandomInteger::put_out(ostream& os) const
  {
    return os << characteristics_->first << ":"
        << characteristics_->second.size_ << ":" << value_;
  }

} /* namespace cpprob */

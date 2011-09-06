/*
 * CategoricalDistribution.cpp
 *
 *  Created on: 28.07.2011
 *      Author: wbam
 */

#include "CategoricalDistribution.hpp"

namespace cpprob
{

  std::ostream&
  operator<<(std::ostream& os, const CategoricalDistribution& d)
  {
    os << "     ";
    for (CategoricalDistribution::const_iterator p = d.begin(); p != d.end();
        ++p)
      os << " (" << p->first << "," << p->second << ") ";
    os << "\n";
    return os;
  }

  void
  CategoricalDistribution::normalize()
  {
    float sum = 0.0f;
    for (iterator i = begin(); i != end(); ++i)
      sum += i->second;

    float scale_factor = 1.0f / sum;
    for (iterator i = begin(); i != end(); ++i)
      i->second *= scale_factor;
  }

}

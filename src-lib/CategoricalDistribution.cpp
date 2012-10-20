/*
 * CategoricalDistribution.cpp
 *
 *  Created on: 28.07.2011
 *      Author: wbam
 */

#include "CategoricalDistribution.hpp"

using namespace std;

namespace cpprob
{

  vector<float>
  difference(const CategoricalDistribution& minuend,
      const CategoricalDistribution& subtrahend)
  {
    vector<float> result;

    auto minuend_it = minuend.begin();
    auto subtrahend_it = subtrahend.begin();
    for (; minuend_it != minuend.end() && subtrahend_it != subtrahend.end();
        ++minuend_it, ++subtrahend_it)
      result.push_back(minuend_it->second - subtrahend_it->second);

    return result;
  }

  float
  mean(const CategoricalDistribution& d)
  {
    float m = 0.0;
    float value = 0.0;

    for (auto it = d.begin(); it != d.end(); ++it, ++value)
      m += value * it->second;

    return m;
  }

  ostream&
  operator<<(ostream& os, const CategoricalDistribution& d)
  {
    os << "     ";
    for (CategoricalDistribution::const_iterator p = d.begin(); p != d.end();
        ++p)
      os << " (" << p->first << "," << p->second << ") ";
    os << "\n";
    return os;
  }

  float
  standard_deviation(const CategoricalDistribution& d)
  {
    return sqrt(variance(d));
  }

  float
  variance(const CategoricalDistribution& d)
  {
    float v = 0.0;
    float diff = -mean(d);

    for (auto it = d.begin(); it != d.end(); ++it, ++diff)
      v += diff * diff * it->second;

    return v;
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

/*
 * dirichlet_distribution.cpp
 *
 *  Created on: 22.07.2011
 *      Author: wbam
 */

#include "dirichlet_distribution.hpp"
#include "io_utils.hpp"
#include <algorithm>

using namespace std;

namespace vanet
{

  DirichletDistribution::DirichletDistribution(const RandomProbabilities& var,
      float alpha)
  {
    for (RandomProbabilities::const_iterator p = var.begin(); p != var.end();
        ++p)
      parameters_.insert(make_pair(p->first, alpha));
  }

  DirichletDistribution::~DirichletDistribution()
  {
  }

  ostream&
  operator<<(ostream& os, const DirichletDistribution& dd)
  {
    for_each(dd.parameters_.begin(), dd.parameters_.end(),
        StreamOut(os, " ", ""));
    return os;
  }

}

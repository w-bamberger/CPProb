/*
 * cond_dirichlet_distribution.cpp
 *
 *  Created on: 25.07.2011
 *      Author: wbam
 */

#include "cond_dirichlet_distribution.hpp"

using namespace std;

namespace cpprob
{
  std::ostream&
  operator<<(std::ostream& os, const CondDirichletDistribution& cdd)
  {
    for (CondDirichletDistribution::Distributions::const_iterator d_it =
        cdd.distributions_.begin(); d_it != cdd.distributions_.end(); ++d_it)
      {
      os << "      Condition " << d_it->first << ", parameters " << d_it->second;

    }
  return os;
}

CondDirichletDistribution::CondDirichletDistribution(
    const RandomConditionalProbabilities cpt, float alpha)
{
  for (RandomConditionalProbabilities::const_iterator pt = cpt.begin();
      pt != cpt.end(); ++pt)
        {
    distributions_.insert(
        make_pair(pt->first, DirichletDistribution(pt->second, alpha)));
  }
}

CondDirichletDistribution::~CondDirichletDistribution()
{
}

}

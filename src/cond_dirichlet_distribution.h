/*
 * cond_dirichlet_distribution.h
 *
 *  Created on: 25.07.2011
 *      Author: wbam
 */

#ifndef COND_DIRICHLET_DISTRIBUTION_H_
#define COND_DIRICHLET_DISTRIBUTION_H_

#include "dirichlet_distribution.h"
#include "random_conditional_probabilities.h"

namespace vanet
{

  class CondDirichletDistribution
  {

  public:

    typedef std::map<DiscreteRandomVariable, DirichletDistribution>
        Distributions;
    typedef DirichletDistribution::input_type input_type;
    typedef RandomConditionalProbabilities result_type;

    CondDirichletDistribution(const RandomConditionalProbabilities cpt,
        float alpha);

    virtual
    ~CondDirichletDistribution();

    const DirichletDistribution&
    at(const DiscreteRandomVariable& condition) const
    {
      Distributions::const_iterator find_it = distributions_.find(condition);
      if (find_it != distributions_.end())
        return find_it->second;
      else
        throw std::runtime_error("CondDirichletDistribution: Unknown condition");
    }

    DirichletDistribution&
    at(const DiscreteRandomVariable& condition)
    {
      Distributions::iterator find_it = distributions_.find(condition);
      if (find_it != distributions_.end())
        return find_it->second;
      else
        throw std::runtime_error("CondDirichletDistribution: Unknown condition");
    }

    template<class _RandomNumberEngine>
      result_type
      operator()(_RandomNumberEngine& rne)
      {
        const DiscreteRandomVariable& condition = distributions_.begin()->first;
        const DiscreteRandomVariable& var =
            distributions_.begin()->second.parameters().begin()->first;
        RandomConditionalProbabilities result(var, condition);

        RandomConditionalProbabilities::iterator p = result.begin();
        Distributions::iterator d = distributions_.begin();
        for (; p != result.end(); ++p, ++d)
            p->second = d->second.operator ()(rne);

        return result;
      }

    friend std::ostream&
    operator<<(std::ostream& os, const CondDirichletDistribution& cdd);

  private:

    Distributions distributions_;

  };

}

#endif /* COND_DIRICHLET_DISTRIBUTION_H_ */

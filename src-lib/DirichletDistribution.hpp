/*
 * DirichletDistribution.hpp
 *
 *  Created on: 22.07.2011
 *      Author: wbam
 */

#ifndef DIRICHLETDISTRIBUTION_HPP_
#define DIRICHLETDISTRIBUTION_HPP_

#include "RandomProbabilities.hpp"

namespace cpprob
{

  class DirichletDistribution
  {

  public:

    typedef std::map<DiscreteRandomVariable, float> Parameters;
    typedef std::gamma_distribution<float>::input_type input_type;
    typedef RandomProbabilities result_type;

    DirichletDistribution(const RandomProbabilities& var, float alpha);

    template<class Iterator>
      DirichletDistribution(const Iterator& begin, const Iterator& end)
          : parameters_(begin, end)
      {
      }

    ~DirichletDistribution();

    const Parameters&
    parameters() const
    {
      return parameters_;
    }

    Parameters&
    parameters()
    {
      return parameters_;
    }

    template<class RandomNumberEngine>
      RandomProbabilities
      operator()(RandomNumberEngine& rne)
      {
        float sum = 0.0;
        RandomProbabilities result(parameters_.begin()->first);
        RandomProbabilities::iterator i = result.begin();
        Parameters::const_iterator param = parameters_.begin();

        for (; i != result.end(); ++i, ++param)
          {
          std::gamma_distribution<float> gd = std::gamma_distribution<
              float>(param->second);
          i->second = gd(rne);
          sum += i->second;
        }
      for (i = result.begin(); i != result.end(); ++i)
        i->second /= sum;

      return result;
    }

  friend std::ostream&
  operator<<(std::ostream& os, const DirichletDistribution& dd);

private:

  Parameters parameters_;

  };

}

#endif /* DIRICHLETDISTRIBUTION_HPP_ */

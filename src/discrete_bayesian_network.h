/*
 * bayesian_network.h
 *
 *  Created on: 16.05.2011
 *      Author: wbam
 */

#ifndef DISCRETE_BAYESIAN_NETWORK_H_
#define DISCRETE_BAYESIAN_NETWORK_H_

#include "categorical_distribution.h"
#include "conditional_categorical_distribution.h"
#include "count_distribution.h"
#include <boost/graph/adjacency_list.hpp>

namespace vanet
{

  struct VertexProperties
  {

    DiscreteRandomVariable random_variable;
    ConditionalCategoricalDistribution distribution;
    bool value_is_evidence;

    VertexProperties() :
      distribution(random_variable, std::vector<const DiscreteRandomVariable*>()), value_is_evidence(false)
    {
    }

    VertexProperties&
    operator=(const VertexProperties& other_vp)
    {
      random_variable = other_vp.random_variable;
      distribution.clear();
      ConditionalCategoricalDistribution::const_iterator other_it = other_vp.distribution.begin();
      ConditionalCategoricalDistribution::iterator my_it = distribution.begin();
      for (; other_it != other_vp.distribution.end(); ++other_it)
        {

        }
      std::copy(other_vp.distribution.begin(), other_vp.distribution.end(), std::inserter(distribution, distribution.begin()));
      value_is_evidence = other_vp.value_is_evidence;
      return *this;
    }

  };

  typedef boost::adjacency_list<boost::vecS, boost::vecS,
      boost::bidirectionalS, VertexProperties, boost::no_property>
      DiscreteBayesianNetwork;

  std::ostream&
  operator<<(std::ostream& os, const DiscreteBayesianNetwork& bn);

  CategoricalDistribution
  enumerate(DiscreteBayesianNetwork& bn,
      DiscreteBayesianNetwork::vertex_descriptor X_v);

  CategoricalDistribution
  gibbs_sampling(DiscreteBayesianNetwork& bn,
      const DiscreteBayesianNetwork::vertex_descriptor& X_v,
      unsigned int iterations = 100);

  void
  learn_multinomial_ml(DiscreteBayesianNetwork& bn,
      const std::vector<std::map<std::string, std::string> >& data);

  void
  learn_multinomial_map(DiscreteBayesianNetwork& bn,
      const std::vector<std::map<std::string, std::string> >& data,
      const std::vector<ConditionalCountDistribution>& prior);

  DiscreteJointRandomVariable
  parents_variable(const DiscreteBayesianNetwork& bn,
      DiscreteBayesianNetwork::vertex_descriptor v);

}

#endif /* DISCRETE_BAYESIAN_NETWORK_H_ */

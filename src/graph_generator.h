/*
 * graph_generator.h
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef GRAPH_GENERATOR_H_
#define GRAPH_GENERATOR_H_

#include "discrete_bayesian_network.h"
#include "hybrid_bayesian_network.h"
#include <numeric>

namespace vanet
{

  class GraphGenerator
  {

  public:

    GraphGenerator();

    ~GraphGenerator();

    static HybridBayesianNetwork
    gen_alarm_net();

    static DiscreteBayesianNetwork
    gen_bag_net();

    static HybridBayesianNetwork
    gen_bag_net_hybrid(float alpha, size_t line_of_evidence =
        std::numeric_limits<std::size_t>::max());

    static DiscreteBayesianNetwork
    gen_naive_bayes_trust_net();

  };

}

#endif /* GRAPH_GENERATOR_H_ */

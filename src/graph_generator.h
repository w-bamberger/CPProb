/*
 * graph_generator.h
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef GRAPH_GENERATOR_H_
#define GRAPH_GENERATOR_H_

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

    static HybridBayesianNetwork
    gen_bag_net(float alpha, size_t line_of_evidence =
        std::numeric_limits<std::size_t>::max());

  };

}

#endif /* GRAPH_GENERATOR_H_ */

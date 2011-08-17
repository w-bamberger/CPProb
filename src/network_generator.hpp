/*
 * network_generator.hpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef NETWORK_GENERATOR_HPP_
#define NETWORK_GENERATOR_HPP_

#include "bayesian_network.hpp"
#include <numeric>

namespace vanet
{

  class NetworkGenerator
  {

  public:

    NetworkGenerator();

    ~NetworkGenerator();

    static BayesianNetwork
    gen_alarm_net();

    static BayesianNetwork
    gen_bag_net(float alpha, size_t line_of_evidence =
        std::numeric_limits<std::size_t>::max());

  };

}

#endif /* NETWORK_GENERATOR_HPP_ */

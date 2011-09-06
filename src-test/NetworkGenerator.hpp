/*
 * NetworkGenerator.hpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef NETWORKGENERATOR_HPP_
#define NETWORKGENERATOR_HPP_

#include "../src-lib/BayesianNetwork.hpp"
#include <numeric>

namespace cpprob
{

  class NetworkGenerator
  {

  public:

    NetworkGenerator();

    ~NetworkGenerator();

    static BayesianNetwork
    gen_alarm_net();

    static BayesianNetwork
    gen_bag_net(float alpha, bool fully_observed, size_t line_of_evidence =
        std::numeric_limits<std::size_t>::max());

  };

}

#endif /* NETWORKGENERATOR_HPP_ */

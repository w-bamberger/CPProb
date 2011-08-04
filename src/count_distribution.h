/*
 * count_distribution.h
 *
 *  Created on: 08.07.2011
 *      Author: wbam
 */

#ifndef COUNT_DISTRIBUTION_H_
#define COUNT_DISTRIBUTION_H_

#include "discrete_random_variable.h"
#include <map>

namespace vanet
{

typedef std::map<DiscreteRandomVariable, std::size_t> CountDistribution;
typedef std::map<std::pair<DiscreteRandomVariable, DiscreteRandomVariable>,
    std::size_t> ConditionalCountDistribution;

}

#endif /* COUNT_DISTRIBUTION_H_ */

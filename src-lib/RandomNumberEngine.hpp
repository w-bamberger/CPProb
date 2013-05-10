/*
 * RandomNumberEngine.hpp
 *
 *  Created on: 11.07.2011
 *      Author: wbam
 */

#ifndef RANDOMNUMBERENGINE_HPP_
#define RANDOMNUMBERENGINE_HPP_

#include "ListNumberGenerator.hpp"
#include <random>

namespace cpprob
{

#ifdef CPPROB_RANDOM_NUMBER_ENGINE

  typedef CPPROB_RANDOM_NUMBER_ENGINE RandomNumberEngine;

#else

  typedef std::mt19937 RandomNumberEngine;

#endif

  extern RandomNumberEngine random_number_engine;

}

#endif /* RANDOMNUMBERENGINE_HPP_ */

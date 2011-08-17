/*
 * random_number_engine.hpp
 *
 *  Created on: 11.07.2011
 *      Author: wbam
 */

#ifndef RANDOM_NUMBER_ENGINE_HPP_
#define RANDOM_NUMBER_ENGINE_HPP_

#include <tr1/random>

namespace cpprob
{

  typedef std::tr1::mt19937 RandomNumberEngine;
  extern RandomNumberEngine random_number_engine;

}

#endif /* RANDOM_NUMBER_ENGINE_HPP_ */

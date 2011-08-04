/*
 * random_number_engine.h
 *
 *  Created on: 11.07.2011
 *      Author: wbam
 */

#ifndef RANDOM_NUMBER_ENGINE_H_
#define RANDOM_NUMBER_ENGINE_H_

#include <tr1/random>

namespace vanet
{

  typedef std::tr1::mt19937 RandomNumberEngine;
  extern RandomNumberEngine random_number_engine;

}

#endif /* RANDOM_NUMBER_ENGINE_H_ */

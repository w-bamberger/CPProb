/*
 * RandomNumberEngine.hpp
 *
 *  Created on: 11.07.2011
 *      Author: wbam
 */

#ifndef RANDOMNUMBERENGINE_HPP_
#define RANDOMNUMBERENGINE_HPP_

#include <tr1/random>

namespace cpprob
{

  typedef std::tr1::mt19937 RandomNumberEngine;
  extern RandomNumberEngine random_number_engine;

}

#endif /* RANDOMNUMBERENGINE_HPP_ */

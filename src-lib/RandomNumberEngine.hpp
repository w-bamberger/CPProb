/*
 * RandomNumberEngine.hpp
 *
 *  Created on: 11.07.2011
 *      Author: wbam
 */

#ifndef RANDOMNUMBERENGINE_HPP_
#define RANDOMNUMBERENGINE_HPP_

#include <random>

namespace cpprob
{

  typedef std::mt19937 RandomNumberEngine;
  extern RandomNumberEngine random_number_engine;

}

#endif /* RANDOMNUMBERENGINE_HPP_ */

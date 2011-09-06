/*
 * Error.cpp
 *
 *  Created on: 28.07.2011
 *      Author: wbam
 */

#include "Error.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

namespace cpprob
{

  void
  program_failed(const string& msg)
  {
    cerr << msg << endl;
    terminate();
  }

  NetworkError::NetworkError(const std::string& msg) :
      msg_(msg)
  {
  }

  NetworkError::~NetworkError() throw ()
  {
  }

  const char*
  NetworkError::what() const throw ()
  {
    return msg_.c_str();
  }

}

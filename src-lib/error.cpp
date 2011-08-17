/*
 * error.cpp
 *
 *  Created on: 28.07.2011
 *      Author: wbam
 */

#include "error.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

namespace vanet
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

/*
 * Error.hpp
 *
 *  Created on: 28.07.2011
 *      Author: wbam
 */

#ifndef ERROR_HPP_
#define ERROR_HPP_

#include <sstream>
#include <stdexcept>

namespace cpprob
{

  void
  program_failed(const std::string& msg);

  class NetworkError : public std::exception
  {

  public:

    explicit
    NetworkError(const std::string& msg);

    virtual
    ~NetworkError() throw ();

    virtual const char*
    what() const throw ();

  private:

    std::string msg_;

  };

}

#define cpprob_throw(exception_name, msg) \
   do { \
        std::ostringstream oss; \
        oss << msg; \
        throw exception_name(oss.str().c_str()); \
   } while (false)

#define cpprob_throw_invalid_argument(msg) \
    cpprob_throw(std::invalid_argument, msg)

#define cpprob_throw_logic_error(msg) \
    cpprob_throw(std::logic_error, msg)

#define cpprob_throw_network_error(msg) \
    cpprob_throw(cpprob::NetworkError, msg)

#define cpprob_throw_out_of_range(msg) \
    cpprob_throw(std::out_of_range, msg)

#define cpprob_throw_runtime_error(msg) \
    cpprob_throw(std::runtime_error, msg)

#ifdef CPPROB_DEBUG_MODE
#define cpprob_check_debug(condition, msg) \
   do { \
      if (!(condition)) \
      { \
         std::ostringstream oss; \
         oss << "Assertion `" #condition "` failed in " << __FILE__ \
            << " at line " << __LINE__ << ": " << msg; \
         cpprob::program_failed(oss.str()); \
      } \
   } while (false)
#else
#define cpprob_check_debug(condition, msg) do {} while (false)
#endif

#endif /* ERROR_HPP_ */

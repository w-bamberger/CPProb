/*
 * error.hpp
 *
 *  Created on: 28.07.2011
 *      Author: wbam
 */

#ifndef ERROR_HPP_
#define ERROR_HPP_

#include <sstream>
#include <stdexcept>

namespace vanet
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

#define vanet_throw(msg, exception_name) \
   do { \
        std::ostringstream oss; \
        oss << msg; \
        throw exception_name(oss.str().c_str()); \
   } while (false)

#define vanet_throw_logic_error(msg) \
    vanet_throw(msg, std::logic_error)

#define vanet_throw_network_error(msg) \
    vanet_throw(msg, vanet::NetworkError)

#define vanet_throw_out_of_range(msg) \
    vanet_throw(msg, std::out_of_range)

#define vanet_throw_runtime_error(msg) \
    vanet_throw(msg, std::runtime_error)

#ifdef VANET_DEBUG_MODE
#define vanet_check_debug(condition, msg) \
   do { \
      if (!(condition)) \
      { \
         std::ostringstream oss; \
         oss << "Assertion `" #condition "` failed in " << __FILE__ \
            << " at line " << __LINE__ << ": " << msg; \
         vanet::program_failed(oss.str()); \
      } \
   } while (false)
#else
#define vanet_check_debug(condition, msg) do {} while (false)
#endif

#endif /* ERROR_HPP_ */

/*
 * debug_algorithms.hpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef IO_UTILS_HPP_
#define IO_UTILS_HPP_

#include <ostream>

namespace vanet
{

  template<class T1, class T2>
    inline std::ostream&
    operator<<(std::ostream& os, const std::pair<T1, T2>& p)
    {
      return os << "(" << p.first << " " << p.second << ")";
    }

  class StreamOut
  {

  public:

    explicit
    StreamOut(std::ostream& os, const std::string& prefix = "",
        const std::string& postfix = "\n") :
        os_(os), prefix_(prefix), postfix_(postfix)
    {
    }

    template<class T>
      void
      operator()(const T& t)
      {
        os_ << prefix_ << t << postfix_;
      }

  private:

    std::ostream& os_;
    const std::string prefix_;
    const std::string postfix_;

  };

}

#endif /* IO_UTILS_HPP_ */

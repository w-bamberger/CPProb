/*
 * Test.hpp
 *
 *  Created on: 10.10.2011
 *      Author: wbam
 */

#ifndef TEST_HPP_
#define TEST_HPP_
#include <boost/test/unit_test.hpp>

namespace cpprob
{
  template<class V1, class V2>
    void
    check_equal(const std::string& string1, const V1& value1,
        const std::string& string2, const V2& value2)
    {
      BOOST_CHECK_MESSAGE(
          value1 == value2,
          string1 << " (" << value1 << ") == " << string2 << " (" << value2 << ")");
    }
}

#define CPPROB_CHECK_EQUAL(a, b) cpprob::check_equal(#a, a, #b, b)

#endif /* TEST_HPP_ */

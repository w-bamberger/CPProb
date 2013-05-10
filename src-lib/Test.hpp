/*
 * Test.hpp
 *
 *  Created on: 10.10.2011
 *      Author: wbam
 */

#ifndef CPPROB_TEST_HPP_
#define CPPROB_TEST_HPP_

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

  template<class R1, class R2>
    void
    check_equal_ranges(const std::string& string1, const R1& range1,
        const std::string& string2, const R2& range2)
    {
      auto r1_it = range1.begin();
      auto r2_it = range2.begin();
      auto r1_end = range1.end();
      auto r2_end = range2.end();

      for (; r1_it != r1_end && r2_it != r2_end; ++r1_it, ++r2_it)
      {
        BOOST_CHECK_MESSAGE(
            *r1_it == *r2_it,
            string1 << " (" << *r1_it << ") == " << string2 << " ("<< *r2_it << ")");
      }

      BOOST_CHECK_MESSAGE(r1_it == r1_end, "Reached end of " << string1);
      BOOST_CHECK_MESSAGE(r2_it == r2_end, "Reached end of " << string2);
    }

  inline void
  set_checkpoint(const std::string& file, std::size_t line_number,
      const std::string& message)
  {
    boost::unit_test::unit_test_log.set_checkpoint(file, line_number, message);
    BOOST_TEST_MESSAGE(message);
  }
}

#define CPPROB_CHECKPOINT( M )                             \
    cpprob::set_checkpoint(__FILE__, __LINE__,             \
                           (::boost::wrap_stringstream().ref() << M).str() )

#define CPPROB_CHECK_EQUAL(a, b)                           \
    cpprob::check_equal(#a, a, #b, b)

#define CPPROB_CHECK_EQUAL_RANGES(a, b)                    \
    cpprob::check_equal_ranges(#a, a, #b, b)

#endif /* CPPROB_TEST_HPP_ */

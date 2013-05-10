/*
 * ListNumberGeneratorTest.cpp
 *
 *  Created on: 05.05.2013
 *      Author: wbam
 */

#include "../src-lib/ListNumberGenerator.hpp"
#include <boost/test/unit_test.hpp>
#include <random>

using namespace cpprob;
using namespace std;

BOOST_AUTO_TEST_SUITE(ListNumberGeneratorTest)

/* From the GCC version 4.8 standard library. In my old one,
 * it is not yet in the random header.
 * This function can be removed, when using a newer standard compliant library.
 */
template<typename _RealType, size_t __bits,
typename _UniformRandomNumberGenerator>
_RealType
generate_canonical(_UniformRandomNumberGenerator& __urng)
{
  const size_t __b = std::min(
      static_cast<size_t>(std::numeric_limits<_RealType>::digits), __bits);
  const long double __r = static_cast<long double>(__urng.max())
  - static_cast<long double>(__urng.min()) + 1.0L;
  const size_t __log2r = std::log(__r) / std::log(2.0L);
  size_t __k = std::max<size_t>(1UL, (__b + __log2r - 1UL) / __log2r);
  _RealType __sum = _RealType(0);
  _RealType __tmp = _RealType(1);
  for (; __k != 0; --__k)
  {
    __sum += _RealType(__urng() - __urng.min()) * __tmp;
    __tmp *= __r;
  }
  return __sum / __tmp;
}

BOOST_AUTO_TEST_CASE(MinMax)
{
  typedef ListNumberGenerator::result_type result_type;

  BOOST_CHECK_EQUAL(ListNumberGenerator::min(), numeric_limits<result_type>::min());
  BOOST_CHECK_EQUAL(ListNumberGenerator::max(), numeric_limits<result_type>::max());
}

BOOST_AUTO_TEST_CASE(ConstructionSample)
{
  typedef ListNumberGenerator::result_type result_type;

  ListNumberGenerator g1;
  BOOST_CHECK_EQUAL(g1.size(), 0);
  BOOST_CHECK_THROW(g1(), runtime_error);

  ListNumberGenerator g2(numeric_limits<result_type>::max());
  BOOST_CHECK_EQUAL(g2.size(), 1);
  BOOST_CHECK_EQUAL(g2(), numeric_limits<result_type>::max());
  BOOST_CHECK_EQUAL(g2.size(), 0);
  BOOST_CHECK_THROW(g2(), runtime_error);

  ListNumberGenerator g3(
      { numeric_limits<result_type>::min(), 10u, 987654321u, numeric_limits<result_type>::max()});
  BOOST_CHECK_EQUAL(g3.size(), 4);
  BOOST_CHECK_EQUAL(g3(), numeric_limits<result_type>::min());
  BOOST_CHECK_EQUAL(g3.size(), 3);
  BOOST_CHECK_EQUAL(g3(), 10u);
  BOOST_CHECK_EQUAL(g3.size(), 2);
  BOOST_CHECK_EQUAL(g3(), 987654321u);
  BOOST_CHECK_EQUAL(g3.size(), 1);
  BOOST_CHECK_EQUAL(g3(), numeric_limits<result_type>::max());
  BOOST_CHECK_EQUAL(g3.size(), 0);
  BOOST_CHECK_THROW(g3(), runtime_error);

  ListNumberGenerator g4 = ListNumberGenerator::from_canonical(0.5f);
  BOOST_CHECK_EQUAL(g4.size(), 1);
  float canonical = generate_canonical<float, numeric_limits<float>::digits, ListNumberGenerator>(g4);
  BOOST_CHECK_EQUAL(canonical, 0.5f);
  BOOST_CHECK_EQUAL(g4.size(), 0);
  BOOST_CHECK_THROW(g4(), runtime_error);

  ListNumberGenerator g5 = ListNumberGenerator::from_canonical(
      { 0.0f, 0.5f, 1.0f});
  canonical = generate_canonical<float, numeric_limits<float>::digits, ListNumberGenerator>(g5);
  BOOST_CHECK_EQUAL(canonical, 0.0f);
  canonical = generate_canonical<float, numeric_limits<float>::digits, ListNumberGenerator>(g5);
  BOOST_CHECK_EQUAL(canonical, 0.5f);
  canonical = generate_canonical<float, numeric_limits<float>::digits, ListNumberGenerator>(g5);
  BOOST_CHECK_EQUAL(canonical, 1.0f);
  BOOST_CHECK_THROW(g5(), runtime_error);
}

BOOST_AUTO_TEST_CASE(SeedSample)
{
  typedef ListNumberGenerator::result_type result_type;

  ListNumberGenerator g1(100u);
  g1.seed();
  BOOST_CHECK_EQUAL(g1.size(), 0);
  BOOST_CHECK_THROW(g1(), runtime_error);

  ListNumberGenerator g2(100u);
  g2.seed(numeric_limits<result_type>::min());
  BOOST_CHECK_EQUAL(g2.size(), 1);
  BOOST_CHECK_EQUAL(g2(), numeric_limits<result_type>::min());
  BOOST_CHECK_EQUAL(g2.size(), 0);
  BOOST_CHECK_THROW(g2(), runtime_error);

  ListNumberGenerator g3(100u);
  g3.seed(
      { numeric_limits<result_type>::min(), 1000u, numeric_limits<result_type>::max()});
  BOOST_CHECK_EQUAL(g3.size(), 3);
  BOOST_CHECK_EQUAL(g3(), numeric_limits<result_type>::min());
  BOOST_CHECK_EQUAL(g3.size(), 2);
  BOOST_CHECK_EQUAL(g3(), 1000u);
  BOOST_CHECK_EQUAL(g3.size(), 1);
  BOOST_CHECK_EQUAL(g3(), numeric_limits<result_type>::max());
  BOOST_CHECK_EQUAL(g3.size(), 0);
  BOOST_CHECK_THROW(g3(), runtime_error);

  ListNumberGenerator g4(100u);
  g4.seed_from_canonical(0.2f);
  float canonical = generate_canonical<float, numeric_limits<float>::digits, ListNumberGenerator>(g4);
  BOOST_CHECK_EQUAL(canonical, 0.2f);
  BOOST_CHECK_EQUAL(g4.size(), 0);
  BOOST_CHECK_THROW(g4(), runtime_error);

  ListNumberGenerator g5(100u);
  g5.seed_from_canonical(
      { 0.0f, 0.2f, 1.0f});
  BOOST_CHECK_EQUAL(g5.size(), 3);
  canonical = generate_canonical<float, numeric_limits<float>::digits, ListNumberGenerator>(g5);
  BOOST_CHECK_EQUAL(canonical, 0.0f);
  BOOST_CHECK_EQUAL(g5.size(), 2);
  canonical = generate_canonical<float, numeric_limits<float>::digits, ListNumberGenerator>(g5);
  BOOST_CHECK_EQUAL(canonical, 0.2f);
  BOOST_CHECK_EQUAL(g5.size(), 1);
  canonical = generate_canonical<float, numeric_limits<float>::digits, ListNumberGenerator>(g5);
  BOOST_CHECK_EQUAL(canonical, 1.0f);
  BOOST_CHECK_EQUAL(g5.size(), 0);
  BOOST_CHECK_THROW(g5(), runtime_error);
}

BOOST_AUTO_TEST_CASE(DiscardSample)
{
  ListNumberGenerator g1(
      { 10u, 100u, 1000u});
  BOOST_CHECK_EQUAL(g1(), 10u);
  g1.discard();
  BOOST_CHECK_EQUAL(g1(), 1000u);
  BOOST_CHECK_THROW(g1(), runtime_error);
}

BOOST_AUTO_TEST_CASE(Compare)
{
  ListNumberGenerator g1(
      { 10u, 100u, 1000u});
  ListNumberGenerator g2;
  g2.seed(
      { 10u, 100u, 1000u});

  BOOST_CHECK(g1 == g2);
  BOOST_CHECK(! (g1 != g2));

  g2();
  BOOST_CHECK(!(g1 == g2));
  BOOST_CHECK(g1 != g2);
}

BOOST_AUTO_TEST_SUITE_END()

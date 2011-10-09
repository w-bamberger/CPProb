/*
 * CategoricalDistributionTest.cpp
 *
 *  Created on: 09.10.2011
 *      Author: wbam
 */

#include "../src-lib/CategoricalDistribution.hpp"
#include "../src-lib/RandomInteger.hpp"
#include <boost/test/unit_test.hpp>

using namespace cpprob;
using namespace std;

BOOST_AUTO_TEST_SUITE(CategoricalDistributionTest)

class RandomNumberGeneratorMock
{

public:

  RandomNumberGeneratorMock()
  : pos_(values_)
  {
  }

  float
  operator()()
  {
    return *(pos_++);
  }

private:

  static float values_[5];
  float* pos_;

};

float RandomNumberGeneratorMock::values_[5] =
{ 0.0f, 0.05f, 0.1f, 0.2f, 1.0f};

BOOST_AUTO_TEST_CASE(sampling)
{
  RandomInteger var("var", 6, 0);
  RandomInteger var2("var", 6, 2);
  RandomInteger var4("var", 6, 4);
  CategoricalDistribution distribution;
  for (; var != var.value_range().end(); ++var)
  distribution[var] = 0.0;
  distribution[var2] = 0.1;
  distribution[var4] = 0.9;
  BOOST_TEST_MESSAGE("Test distribution:\n    " << distribution);

  RandomNumberGeneratorMock rng;

  DiscreteRandomVariable sample;
  sample = distribution(rng);
  BOOST_TEST_MESSAGE("Sample for 0.00: " << sample);
  BOOST_CHECK(sample == var2);

  sample = distribution(rng);
  BOOST_TEST_MESSAGE("Sample for 0.05: " << sample);
  BOOST_CHECK(sample == var2);

  sample = distribution(rng);
  BOOST_TEST_MESSAGE("Sample for 0.10: " << sample);
  BOOST_CHECK(sample == var2);

  sample = distribution(rng);
  BOOST_TEST_MESSAGE("Sample for 0.20: " << sample);
  BOOST_CHECK(sample == var4);

  sample = distribution(rng);
  BOOST_TEST_MESSAGE("Sample for 1.00: " << sample);
  BOOST_CHECK(sample == var4);
}

BOOST_AUTO_TEST_SUITE_END()

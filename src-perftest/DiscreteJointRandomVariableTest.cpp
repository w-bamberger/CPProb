/*
 * DiscreteJointRandomVariableTest.cpp
 *
 *  Created on: 07.10.2011
 *      Author: wbam
 */

#include "../src-lib/DiscreteJointRandomVariable.hpp"
#include "../src-lib/Test.hpp"
#include "../src-lib/RandomInteger.hpp"

using namespace cpprob;
using namespace std;

BOOST_AUTO_TEST_SUITE(DiscreteJointRandomVariableTest)

BOOST_AUTO_TEST_CASE(construction)
{
  /* Test construction with constructor and insert. */
  RandomInteger condition1("condition1", 2, 0);
  RandomInteger condition2("condition2", 2, 0);
  DiscreteJointRandomVariable joint1(condition1, condition2);
  CPPROB_CHECK_EQUAL(joint1, joint1.value_range().begin());

  DiscreteJointRandomVariable joint2;
  BOOST_CHECK(joint2.insert(condition1).second);
  BOOST_CHECK(joint2.insert(condition2).second);
  CPPROB_CHECK_EQUAL(joint1, joint2);

  /* Test that construction from a variable with a value behind the last is
   * not possible.  */
  ++(++condition1);
  BOOST_TEST_MESSAGE("Test construction from a variable with a value behind the last (" << condition1 << ")");
  BOOST_CHECK_THROW(DiscreteJointRandomVariable joint3(condition1, condition2), invalid_argument);
  DiscreteJointRandomVariable joint4;
  BOOST_CHECK(! joint4.insert(condition1).second);

  /* Test that construction from an empty variable is not possible. */
  DiscreteRandomVariable condition3;
  BOOST_TEST_MESSAGE("Test construction from an empty variable (" << condition3 << ")");
  BOOST_CHECK_THROW(DiscreteJointRandomVariable joint5(condition2, condition3), invalid_argument);
  BOOST_CHECK(! joint4.insert(condition3).second);
}

BOOST_AUTO_TEST_CASE(range)
{
  RandomInteger condition1("condition1", 3, 1);
  RandomInteger condition2("condition2", 4, 2);
  DiscreteJointRandomVariable joint(condition1, condition2);

  BOOST_CHECK_MESSAGE(joint.value_range().size() == condition1.value_range().size() * condition2.value_range().size(), "Value range: " << joint.value_range().size() << " = " << condition1.value_range().size() << " * " << condition2.value_range().size());

  --condition1;
  --(--condition2);
  DiscreteJointRandomVariable joint_begin(condition1, condition2);
  DiscreteRandomVariable begin = joint.value_range().begin();
  CPPROB_CHECK_EQUAL(begin, joint_begin);

  ++(++condition1);
  ++(++(++condition2));
  DiscreteJointRandomVariable joint_last(condition1, condition2);
  DiscreteRandomVariable last = --(joint.value_range().end());
  CPPROB_CHECK_EQUAL(last, joint_last);
}

BOOST_AUTO_TEST_CASE(operators)
{
  RandomInteger condition1("condition1", 3, 0);
  RandomInteger condition2("condition2", 4, 0);
  DiscreteJointRandomVariable joint1(condition1, condition2);
  DiscreteJointRandomVariable joint2;

  /* Test assignment. */
  joint2 = joint1;
  CPPROB_CHECK_EQUAL(joint2, joint1);

  /* Test prefix increment. */
  DiscreteRandomVariable var2 = joint2;
  bool test_succeeded = true;
  for (; joint2 != joint2.value_range().end(); ++joint2, ++var2)
  {
    if (joint2 != var2)
    {
      test_succeeded = false;
    }
  }
  if (joint2 != var2)
  {
    test_succeeded = false;
  }
  if (var2 != var2.value_range().end())
  {
    test_succeeded = false;
  }
  BOOST_CHECK_MESSAGE(test_succeeded, "Test prefix increment with loop");

  /* Test prefix decrement. */
  test_succeeded = true;
  for (; joint2 != joint2.value_range().begin(); --joint2, --var2)
  {
    if (joint2 != var2)
    {
      test_succeeded = false;
    }
  }
  if (joint2 != var2)
  {
    test_succeeded = false;
  }
  if (var2 != var2.value_range().begin())
  {
    test_succeeded = false;
  }
  BOOST_CHECK_MESSAGE(test_succeeded, "Test prefix decrement with loop");

  /* Test prefix and postfix increment and decrement. */
  joint2 = joint1;
  CPPROB_CHECK_EQUAL(--(++joint2), joint1);
  CPPROB_CHECK_EQUAL(joint2++, joint1);
  CPPROB_CHECK_EQUAL(joint2--, ++joint1);
  CPPROB_CHECK_EQUAL(joint2, --joint1);

  /* Test inequality */
  BOOST_CHECK(!(joint2 != joint1));
  BOOST_CHECK(joint2 != ++joint1);
}

BOOST_AUTO_TEST_CASE(set_methods)
{
  RandomInteger condition1("condition1", 3, 1);
  RandomInteger condition2("condition2", 4, 2);
  DiscreteJointRandomVariable joint1(condition1, condition2);

  DiscreteJointRandomVariable::iterator it = joint1.begin();
  CPPROB_CHECK_EQUAL(*it, condition1);
  ++it;
  CPPROB_CHECK_EQUAL(*it, condition2);
  ++it;
  BOOST_CHECK_MESSAGE(it == joint1.end(), "Test the iterator to the end of the variable set");

  BOOST_CHECK(joint1.contains(condition1));
  BOOST_CHECK(joint1.contains(condition2));

  CPPROB_CHECK_EQUAL(*joint1.find("condition1"), condition1);
  CPPROB_CHECK_EQUAL(*joint1.find("condition2"), condition2);
}

BOOST_AUTO_TEST_SUITE_END()

/*
 * DiscreteRandomVariableMapTest.cpp
 *
 *  Created on: 08.11.2011
 *      Author: wbam
 */

#include "../src-lib/DiscreteRandomVariableMap.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include "../src-lib/RandomInteger.hpp"
#include <boost/test/unit_test.hpp>

using namespace cpprob;
using namespace std;

template<class T>
  ostream&
  operator<<(ostream& os, const DiscreteRandomVariableMap<T>& map)
  {
    for (auto it = map.begin(); it != map.end(); ++it)
      os << it->first << " – " << it->second << endl;
    return os;
  }

class DiscreteRandomVariableMapFixture
{

public:

  typedef std::pair<DiscreteRandomVariable, float> ValueType;
  typedef DiscreteRandomVariableMap<float> MapType;

  DiscreteRandomVariableMapFixture()
      : var("Var", 7, 0)
  {
    typedef std::pair<DiscreteRandomVariable, float> V;
    V a[5] =
      { //
        V(var.observation(1), 1.5), //
        V(var.observation(2), 2.5), //
        V(var.observation(3), 3.5), //
        V(var.observation(5), 5.5), //
        V(var.observation(6), 6.5) //
        };

    copy(a, a + array_size_, array_);

    map_[var.observation(1)] = 1.5;
    map_[var.observation(2)] = 2.5;
    map_[var.observation(3)] = 3.5;
    map_[var.observation(5)] = 5.5;
    map_[var.observation(6)] = 6.5;
  }

protected:

  static const size_t array_size_;
  RandomInteger var;
  ValueType array_[5];
  DiscreteRandomVariableMap<float> map_;

};

const size_t DiscreteRandomVariableMapFixture::array_size_ = 5;

BOOST_FIXTURE_TEST_SUITE(DiscreteRandomVariableMapTest, DiscreteRandomVariableMapFixture)

BOOST_AUTO_TEST_CASE(Access)
{
  MapType& m = map_;
  BOOST_CHECK_EQUAL(m.size(), array_size_);
  cout << "Test map" << endl << m << endl;

  var.observation(1);
  BOOST_CHECK_EQUAL(m.at(var), 1.5);
  m.at(var) = -1.5;
  BOOST_CHECK_EQUAL(m.at(var), -1.5);

  BOOST_CHECK_EQUAL(m.at(var.observation(2)), 2.5);
  BOOST_CHECK_EQUAL(m.at(var.observation(3)), 3.5);

  var.observation(4);
  BOOST_CHECK_THROW(m.at(var), std::out_of_range);
  BOOST_CHECK_EQUAL(m.size(), array_size_);
  BOOST_CHECK_EQUAL(m[var], 0.0);
  m[var] = 4.5;
  BOOST_CHECK_EQUAL(m.at(var), 4.5);
  BOOST_CHECK_EQUAL(m[var], 4.5);
  BOOST_CHECK_EQUAL(m.size(), array_size_ + 1);

  BOOST_CHECK_EQUAL(m.at(var.observation(5)), 5.5);
  BOOST_CHECK_EQUAL(m[var.observation(6)], 6.5);

  var.observation(0);
  m[var] = -1.0;
  BOOST_CHECK_EQUAL(m.at(var), -1.0);
  BOOST_CHECK_EQUAL(m[var], -1.0);

  BOOST_CHECK_EQUAL(m.size(), array_size_ + 2);
}

BOOST_AUTO_TEST_CASE(Const_access)
{
  const MapType& m = map_;
  BOOST_CHECK_EQUAL(m.size(), array_size_);
  cout << "Test map" << endl << m << endl;

  var.observation(1);
  BOOST_CHECK_EQUAL(m.at(var), 1.5);

  BOOST_CHECK_EQUAL(m.at(var.observation(2)), 2.5);
  BOOST_CHECK_EQUAL(m.at(var.observation(3)), 3.5);

  var.observation(4);
  BOOST_CHECK_THROW(m.at(var), std::out_of_range);

  BOOST_CHECK_EQUAL(m.at(var.observation(5)), 5.5);
  BOOST_CHECK_EQUAL(m.at(var.observation(6)), 6.5);

  BOOST_CHECK_THROW(m.at(var.observation(0)), std::out_of_range);

  BOOST_CHECK_EQUAL(m.size(), array_size_);
}

BOOST_AUTO_TEST_CASE(Assignment)
{
  /* Self-assignment. */
  BOOST_TEST_CHECKPOINT("Self-assignment.");
  MapType m1 = map_;
  MapType& m2 = m1;
  m1 = m2;
  BOOST_CHECK(equal(m1.begin(), m1.end(), map_.begin()));

  /* Assignment from initializer list. */
  BOOST_TEST_CHECKPOINT("Assignment from initializer list.");
  m1 =
  {
    { var, 0.4},
    { var.observation(true), 0.6}
  };

  BOOST_CHECK_EQUAL(m1.size(), 2);
  BOOST_CHECK(!m1.empty());
  BOOST_CHECK(++(++m1.begin()) == m1.end());
  BOOST_CHECK(m1.begin() == --(--m1.end()));

  /* Copy assignment (from fill or empty state). */
  BOOST_TEST_CHECKPOINT("Copy assignment (from filled or empty state).");
  m1 = map_;
  BOOST_CHECK(equal(m1.begin(), m1.end(), map_.begin()));
  MapType m3;
  m3 = m1;
  BOOST_CHECK(equal(m3.begin(), m3.end(), map_.begin()));

  /* Move assignment (from fill or empty state). */
  BOOST_TEST_CHECKPOINT("Move assignment (from fill or empty state).");
  MapType m4 = map_;
  m4 = std::move(m1);
  BOOST_CHECK(equal(m4.begin(), m4.end(), map_.begin()));
  MapType m5;
  m5 = std::move(m4);
  BOOST_CHECK(equal(m5.begin(), m5.end(), map_.begin()));
}

BOOST_AUTO_TEST_CASE(Capacity)
{
  RandomBoolean var("Var", false);

  MapType m;
  BOOST_CHECK_EQUAL(m.size(), 0);
  BOOST_CHECK(m.empty());
  BOOST_CHECK(m.begin() == m.end());
  BOOST_CHECK_NE(m.max_size(), 0);

  m =
  {
    { var, 0.4},
    { var.observation(true), 0.6}
  };
  BOOST_CHECK_EQUAL(m.size(), 2);
  cout << "Test map" << endl << m << endl;
  BOOST_CHECK(!m.empty());
  BOOST_CHECK_NE(m.max_size(), 0);
  BOOST_CHECK(++(++m.begin()) == m.end());
  BOOST_CHECK(m.begin() == --(--m.end()));

  m.clear();
  BOOST_CHECK(m.empty());
  BOOST_CHECK_EQUAL(m.size(), 0);
  BOOST_CHECK(m.begin() == m.end());

}

BOOST_AUTO_TEST_CASE(Construction)
{
  /* Default construction. */
  BOOST_TEST_CHECKPOINT("Default construction.");
  MapType m1;
  BOOST_CHECK_EQUAL(m1.size(), 0);
  BOOST_CHECK(m1.empty());
  BOOST_CHECK(m1.begin() == m1.end());

  /* Construction from initializer list. */
  BOOST_TEST_CHECKPOINT("Construction from initializer list.");
  MapType m2(
      {
        { var.observation(1), 1.5},
        { var.value_range().end(), -0.5}
      });
  BOOST_CHECK_EQUAL(m2.size(), 2);
  BOOST_CHECK(!m2.empty());
  BOOST_CHECK(++(++m2.begin()) == m2.end());
  BOOST_CHECK(m2.begin() == --(--m2.end()));
  BOOST_CHECK_EQUAL(m2.at(var.observation(1)), 1.5);
  BOOST_CHECK_EQUAL(m2.at(var.value_range().end()), -0.5);

  /* Construction from iterators. */
  BOOST_TEST_CHECKPOINT("Construction from iterators.");
  MapType m3(array_, array_ + array_size_);
  // I need to compare with map_, because the elements in array_ have
  // a non-const key.
  BOOST_CHECK(equal(m3.begin(), m3.end(), map_.begin()));

  /* Copy construction. */
  BOOST_TEST_CHECKPOINT("Copy construction.");
  MapType m4(map_);
  BOOST_CHECK(equal(m4.begin(), m4.end(), map_.begin()));

  /* Move construction. */
  BOOST_TEST_CHECKPOINT("Move construction.");
  MapType m5 = std::move(m4);
  BOOST_CHECK(equal(m4.begin(), m4.end(), map_.begin()));

  /* Elements with value_range.size() == 1. */
  BOOST_TEST_CHECKPOINT("Elements with value_range.size() == 1.");
  RandomInteger var1("Var1", 1, 0);
  MapType m6(
      {
        { var1, 0.5}
      });
  BOOST_CHECK_EQUAL(m6.size(), 1);
  BOOST_CHECK(!m6.empty());
  BOOST_CHECK(++m6.begin() == m6.end());
  BOOST_CHECK(m6.begin() == --m6.end());
  BOOST_CHECK_EQUAL(m6.at(var1), 0.5);
  BOOST_CHECK_EQUAL(m6.begin()->second, 0.5);

  /* Element with value_range.size() == 0. */
  BOOST_TEST_CHECKPOINT("Element with value_range.size() == 0.");
  RandomInteger var0("Var0", 0, 0);
  MapType m7(
      {
        { var0.value_range().end(), 0.5}
      });
  BOOST_CHECK_EQUAL(m7.size(), 1);
  BOOST_CHECK(!m7.empty());
  BOOST_CHECK(++m7.begin() == m7.end());
  BOOST_CHECK(m7.begin() == --m7.end());
  BOOST_CHECK_EQUAL(m7.at(var0.value_range().end()), 0.5);
  BOOST_CHECK_EQUAL(m7.begin()->second, 0.5);
}

BOOST_AUTO_TEST_CASE(Iterators)
{
  typedef MapType::iterator iterator;
  typedef MapType::const_iterator const_iterator;

  MapType& m = map_;
  BOOST_CHECK_EQUAL(m.size(), array_size_);
  cout << "Test map" << endl << m << endl;

  /* begin, end, rbegin, rend, operator-> and operator*. */
  BOOST_CHECK_EQUAL(distance(m.begin(), m.end()), array_size_);
  BOOST_CHECK_EQUAL(distance(m.rbegin(), m.rend()), array_size_);
  BOOST_CHECK_EQUAL((*m.begin()).first, (--m.rend())->first);
  BOOST_CHECK_EQUAL((*m.begin()).second, (--m.rend())->second);
  BOOST_CHECK_EQUAL((*m.rbegin()).first, (--m.end())->first);
  BOOST_CHECK_EQUAL((*m.rbegin()).second, (--m.end())->second);

  /* Copy construction, default construction and assignment. */
  iterator i = m.begin();
  iterator j;
  j = i;
  const_iterator ci = i;
  const_iterator cj;
  cj = j;

  /* Comparison of const and non-const iterators. */
  BOOST_CHECK(j == i);
  BOOST_CHECK(j == ci);
  ++j;
  BOOST_CHECK(j != i);
  BOOST_CHECK(j != ci);

  /* Increment like a pointer:
   * pre and postfix increment, operator-> and operator*. */
  ValueType* p = array_;
  while (p != array_ + array_size_ - 1)
  {
    BOOST_CHECK_EQUAL((++p)->first, (++i)->first);
    BOOST_CHECK_EQUAL((*p).second, (*i).second);

    BOOST_CHECK_EQUAL((p++)->first, (i++)->first);
    BOOST_CHECK_EQUAL((*p).second, (*i).second);

    i->second = -1.0;
    BOOST_CHECK_NE((*p).second, (*i).second);
    i->second = p->second;
  }

  /* Decrement like a pointer:
   * pre and postfix decrement, operator-> and operator*. */
  p = array_ + array_size_ - 1;
  i = --m.end();
  while (p != array_)
  {
    BOOST_CHECK_EQUAL((--p)->first, (--i)->first);
    BOOST_CHECK_EQUAL((*p).second, (*i).second);

    BOOST_CHECK_EQUAL((p--)->first, (i--)->first);
    BOOST_CHECK_EQUAL((*p).second, (*i).second);

    (*i).second = -1.0;
    BOOST_CHECK_NE((*p).second, (*i).second);
  }
}

BOOST_AUTO_TEST_CASE(Const_iterators)
{
  typedef MapType::const_iterator const_iterator;

  const MapType& m = map_;
  BOOST_CHECK_EQUAL(m.size(), array_size_);
  cout << "Test map" << endl << m << endl;

  /* begin, end, rbegin, rend, operator-> and operator*. */
  BOOST_CHECK_EQUAL(distance(m.begin(), m.end()), array_size_);
  BOOST_CHECK_EQUAL(distance(m.rbegin(), m.rend()), array_size_);
  BOOST_CHECK_EQUAL(distance(m.cbegin(), m.cend()), array_size_);
  BOOST_CHECK_EQUAL(distance(m.crbegin(), m.crend()), array_size_);
  BOOST_CHECK_EQUAL((*m.begin()).first, (--m.rend())->first);
  BOOST_CHECK_EQUAL((*m.begin()).second, (--m.rend())->second);
  BOOST_CHECK_EQUAL((*m.rbegin()).first, (--m.end())->first);
  BOOST_CHECK_EQUAL((*m.rbegin()).second, (--m.end())->second);
  BOOST_CHECK(m.cbegin() == m.begin());
  BOOST_CHECK(m.cend() == m.end());
  BOOST_CHECK(m.crbegin() == m.rbegin());
  BOOST_CHECK(m.crend() == m.rend());

  /* Copy construction, default construction and assignment. */

  const_iterator i = m.begin();
  const_iterator j;
  j = i;

  /* Comparison. */
  BOOST_CHECK(j == i);
  ++j;
  BOOST_CHECK(j != i);

  /* Increment like a pointer:
   * pre and postfix increment, operator-> and operator*. */
  const ValueType* p = array_;
  while (p != array_ + array_size_ - 1)
  {
    BOOST_CHECK_EQUAL((++p)->first, (++i)->first);
    BOOST_CHECK_EQUAL((*p).second, (*i).second);

    BOOST_CHECK_EQUAL((p++)->first, (i++)->first);
    BOOST_CHECK_EQUAL((*p).second, (*i).second);
  }

  /* Decrement like a pointer:
   * pre and postfix decrement, operator-> and operator*. */
  p = array_ + array_size_ - 1;
  i = --m.end();
  while (p != array_)
  {
    BOOST_CHECK_EQUAL((--p)->first, (--i)->first);
    BOOST_CHECK_EQUAL((*p).second, (*i).second);

    BOOST_CHECK_EQUAL((p--)->first, (i--)->first);
    BOOST_CHECK_EQUAL((*p).second, (*i).second);
  }
}

BOOST_AUTO_TEST_CASE(Modifiers)
{
  /* Clear. */
  MapType m = map_;
  BOOST_CHECK_EQUAL(m.size(), array_size_);
  BOOST_CHECK(!m.empty());
  BOOST_CHECK(m.begin() != m.end());
  m.clear();
  BOOST_CHECK_EQUAL(m.size(), 0);
  BOOST_CHECK(m.empty());
  BOOST_CHECK(m.begin() == m.end());

  /* Insert value by moving. */
  var.observation(1);
  auto insert_result = m.insert(make_pair(var, 1.25));
  BOOST_CHECK(insert_result.second);
  BOOST_CHECK_EQUAL(m.size(), 1);
  BOOST_CHECK_EQUAL(m.at(var), 1.25);
  BOOST_CHECK_EQUAL(insert_result.first->second, 1.25);
  BOOST_CHECK_EQUAL(insert_result.first->first, var);

  --var;
  insert_result = m.insert(make_pair(var, 0.25));
  BOOST_CHECK(insert_result.second);
  BOOST_CHECK_EQUAL(m.size(), 2);
  BOOST_CHECK_EQUAL(m.at(var), 0.25);
  BOOST_CHECK_EQUAL(insert_result.first->second, 0.25);
  BOOST_CHECK_EQUAL(insert_result.first->first, var);

  auto prev_position = insert_result.first;
  insert_result = m.insert(make_pair(var, -1.0));
  BOOST_CHECK_EQUAL(m.size(), 2);
  BOOST_CHECK(!insert_result.second);
  BOOST_CHECK(insert_result.first == prev_position);
  BOOST_CHECK_EQUAL(m.at(var), 0.25);
  BOOST_CHECK_EQUAL(insert_result.first->second, 0.25);
  BOOST_CHECK_EQUAL(insert_result.first->first, var);

  /* Insert value by copying. */
  m.clear();
  var.observation(1);
  const MapType::value_type v1(var, 0.25);
  insert_result = m.insert(v1);
  BOOST_CHECK(insert_result.second);
  BOOST_CHECK_EQUAL(m.size(), 1);
  BOOST_CHECK_EQUAL(m.at(var), 1.25);
  BOOST_CHECK_EQUAL(insert_result.first->second, 1.25);
  BOOST_CHECK_EQUAL(insert_result.first->first, var);

  --var;
  const MapType::value_type v2(var, 0.25);
  insert_result = m.insert(v2);
  BOOST_CHECK(insert_result.second);
  BOOST_CHECK_EQUAL(m.size(), 2);
  BOOST_CHECK_EQUAL(m.at(var), 0.25);
  BOOST_CHECK_EQUAL(insert_result.first->second, 0.25);
  BOOST_CHECK_EQUAL(insert_result.first->first, var);

  prev_position = insert_result.first;
  const MapType::value_type v3(var, -1.0);
  insert_result = m.insert(v3);
  BOOST_CHECK_EQUAL(m.size(), 2);
  BOOST_CHECK(!insert_result.second);
  BOOST_CHECK(insert_result.first == prev_position);
  BOOST_CHECK_EQUAL(m.at(var), 0.25);
  BOOST_CHECK_EQUAL(insert_result.first->second, 0.25);
  BOOST_CHECK_EQUAL(insert_result.first->first, var);
}

BOOST_AUTO_TEST_SUITE_END()

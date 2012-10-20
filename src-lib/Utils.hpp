/*
 * Utils.hpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <algorithm>
#include <cmath>
#include <numeric>

namespace cpprob
{

  template<class OuterOperator, class InnerOperator>
    class ChainFunctors
    {

    public:

      typedef typename InnerOperator::argument_type argument_type;
      typedef typename OuterOperator::result_type result_type;

      ChainFunctors(const OuterOperator& outer_op,
          const InnerOperator& inner_op) :
          outer_op_(outer_op), inner_op_(inner_op)
      {
      }

      result_type
      operator()(const argument_type& a) const
      {
        return outer_op_(inner_op_(a));
      }

    private:

      OuterOperator outer_op_;
      InnerOperator inner_op_;

    };

  template<class OuterOperator, class InnerOperator>
    inline ChainFunctors<OuterOperator, InnerOperator>
    chain_functors(const OuterOperator& outer_op, const InnerOperator& inner_op)
    {
      return ChainFunctors<OuterOperator, InnerOperator>(outer_op, inner_op);
    }

  template<class T>
    class indirect_less : public std::binary_function<T*, T*, bool>
    {

    public:

      bool
      operator()(const T* const x, const T* const y) const
      {
        return *x < *y;
      }

    };

  template<class Range>
    typename Range::value_type
    maximum_norm(const Range& range)
    {
      typename Range::value_type result(0);

      for (auto it = range.begin(); it != range.end(); ++it)
      {
        auto abs_value = std::abs(*it);
        if (result < abs_value)
          result = abs_value;
      }

      return result;
    }

  template<class Iterator, class T>
    void
    normalize_sum(Iterator begin, Iterator end, T target_sum)
    {
      T actual_sum = std::accumulate(begin, end, T(0));
      std::transform(begin, end, begin,
          std::bind2nd(std::multiplies<T>(), target_sum / actual_sum));
    }

  template<class Range, class T>
    void
    normalize_sum(Range range, T target_sum)
    {
      normalize_sum(range.begin(), range.end(), target_sum);
    }

}

#endif /* UTILS_HPP_ */

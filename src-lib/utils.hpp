/*
 * utils.hpp
 *
 *  Created on: 14.05.2011
 *      Author: wbam
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <algorithm>
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

  template<typename _Iterator, typename _Tp>
    void
    normalizeSum(_Iterator begin, _Iterator end, _Tp targetSum)
    {
      _Tp actualSum = std::accumulate(begin, end, _Tp(0));
      std::transform(begin, end, begin,
          std::bind2nd(std::multiplies<_Tp>(), targetSum / actualSum));
    }

}

#endif /* UTILS_HPP_ */

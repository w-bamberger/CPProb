/*
 * ListNumberGenerator.hpp
 *
 *  Created on: 05.05.2013
 *      Author: wbam
 */

#ifndef LISTNUMBERGENERATOR_HPP_
#define LISTNUMBERGENERATOR_HPP_

#include <deque>
#include <limits>

namespace cpprob
{

  /*
   *
   */
  class ListNumberGenerator
  {

  public:

    typedef unsigned int result_type;
    typedef std::deque<result_type>::size_type size_type;

    ListNumberGenerator()
    {
    }

    explicit
    ListNumberGenerator(unsigned int number)
        : numbers_(1, number)
    {
    }

    ListNumberGenerator(std::initializer_list<unsigned int> l)
        : numbers_(l)
    {
    }

    void
    discard()
    {
      numbers_.pop_front();
    }

    static ListNumberGenerator
    from_canonical(float canonical);

    static ListNumberGenerator
    from_canonical(std::initializer_list<float> canonicals);

    // @TODO Should be static constexpr float in C++11
    static unsigned int
    max()
    {
      return std::numeric_limits<unsigned int>::max();
    }

    // @TODO Should be static constexpr float in C++11
    static unsigned int
    min()
    {
      return 0u;
    }

    /**
     * Returns the next number from the number pool.
     *
     * @throw std::runtime_error if the number pool is empty.
     */
    unsigned int
    operator()();

    bool
    operator==(const ListNumberGenerator& y) const
    {
      return numbers_ == y.numbers_;
    }

    bool
    operator!=(const ListNumberGenerator& y) const
    {
      return numbers_ != y.numbers_;
    }

    void
    seed()
    {
      numbers_.clear();
    }

    void
    seed(unsigned int new_number)
    {
      numbers_.clear();
      numbers_.push_back(new_number);
    }

    void
    seed(std::initializer_list<unsigned int> new_numbers)
    {
      numbers_.assign(new_numbers);
    }

    void
    seed_from_canonical(float new_canonical);

    void
    seed_from_canonical(std::initializer_list<float> new_canonicals);

    size_type
    size() const
    {
      return numbers_.size();
    }

  private:

    std::deque<unsigned int> numbers_;

  };

} /* namespace cpprob */

#endif /* LISTNUMBERGENERATOR_HPP_ */

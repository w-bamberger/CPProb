/*
 * RefVector.hpp
 *
 *  Created on: 16.11.2011
 *      Author: wbam
 */

#ifndef REFVECTOR_HPP_
#define REFVECTOR_HPP_

#include "vector.hpp"
#include <boost/iterator/indirect_iterator.hpp>

namespace cpprob
{

  namespace cont
  {

    template<class T>
      class RefVector
      {

        typedef vector<T*> Pointers;

      public:

        typedef T value_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef boost::indirect_iterator<typename Pointers::iterator> iterator;
        typedef boost::indirect_iterator<typename Pointers::const_iterator> const_iterator;
        typedef typename Pointers::size_type size_type;
        typedef typename Pointers::difference_type difference_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        /*
         * Construction and assignment
         */

        RefVector()
            : pointers_()
        {
        }

        RefVector(std::initializer_list<pointer> init_list)
            : pointers_(init_list)
        {
        }

        template<class InputIterator>
          RefVector(InputIterator begin, InputIterator end)
          {
            size_type size = std::distance(begin, end);
            pointers_.reserve(size);
            for (; begin != end; ++begin)
              pointers_.push_back(to_pointer(begin));
          }

        // No destructor: The default destructor is fine. So the implicit
        // move construction and assignment is generated by the compiler.

        /*
         * Iterators
         */

        iterator
        begin()
        {
          return pointers_.begin();
        }

        const_iterator
        begin() const
        {
          return pointers_.begin();
        }

        iterator
        end()
        {
          return pointers_.end();
        }

        const_iterator
        end() const
        {
          return pointers_.end();
        }

        reverse_iterator
        rbegin()
        {
          return reverse_iterator(iterator(pointers_.begin()));
        }

        const_reverse_iterator
        rbegin() const
        {
          return const_reverse_iterator(const_iterator(pointers_.begin()));
        }

        reverse_iterator
        rend()
        {
          return reverse_iterator(iterator(pointers_.end()));
        }

        const_reverse_iterator
        rend() const
        {
          return const_reverse_iterator(const_iterator(pointers_.end()));
        }

        const_iterator
        cbegin() const
        {
          return pointers_.cbegin();
        }

        const_iterator
        cend() const
        {
          return pointers_.cend();
        }

        const_reverse_iterator
        crbegin() const
        {
          return const_reverse_iterator(const_iterator(pointers_.cbegin()));
        }

        const_reverse_iterator
        crend() const
        {
          return const_reverse_iterator(const_iterator(pointers_.cend()));
        }

        void
        push_back(reference x)
        {
          pointers_.push_back(&x);
        }

      private:

        Pointers pointers_;

        template<class BaseIterator>
        pointer
        to_pointer(boost::indirect_iterator<BaseIterator> indirect_it)
        {
          return &(*indirect_it);
        }

        template<class Iterator>
        pointer
        to_pointer(Iterator it)
        {
          return *it;
        }

      };

  } // namespace cont

} // namespace cpprob

#endif /* REFVECTOR_HPP_ */

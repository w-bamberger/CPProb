/*
 * DiscreteRandomVariableMap.hpp
 *
 *  Created on: 04.11.2011
 *      Author: wbam
 */

#ifndef DISCRETERANDOMVARIABLEMAP_HPP_
#define DISCRETERANDOMVARIABLEMAP_HPP_

#include "DiscreteRandomVariable.hpp"
#include "cont/vector.hpp"
#include <memory>

namespace cpprob
{

  /*
   *
   */
  template<class T>
    class DiscreteRandomVariableMap
    {

      struct Element
      {
        Element()
            : prev_offset(0), next_offset(0), data()
        {
        }

        std::ptrdiff_t prev_offset;
        std::ptrdiff_t next_offset;
        std::pair<DiscreteRandomVariable, T> data;
      };

      typedef DiscreteRandomVariableMap<T> Self;
      typedef cont::vector<std::pair<DiscreteRandomVariable, T> > Container;

    public:

      typedef DiscreteRandomVariable key_type;
      typedef T mapped_type;
      typedef std::pair<const DiscreteRandomVariable, T> value_type;
      typedef DiscreteRandomVariable::ValueLess key_compare;
      typedef typename Container::allocator_type allocator_type;
      typedef typename Container::reference reference;
      typedef typename Container::const_reference const_reference;
      typedef typename Container::iterator iterator;
      typedef typename Container::const_iterator const_iterator;
      typedef typename Container::size_type size_type;
      typedef typename Container::difference_type difference_type;
      typedef typename Container::pointer pointer;
      typedef typename Container::const_pointer const_pointer;
      typedef typename Container::reverse_iterator reverse_iterator;
      typedef typename Container::const_reverse_iterator const_reverse_iterator;

      /*
       * Construct, copy and destroy
       */

      DiscreteRandomVariableMap()
      {
      }

      template<class InputIterator>
        DiscreteRandomVariableMap(InputIterator first, InputIterator last)
            : values_(first, last)
        {
          values_.resize(first->frist.value_range());
        }

      DiscreteRandomVariableMap(std::initializer_list<value_type> init_list)
      {
        if (init_list.size() != 0)
        {
          resize(init_list.begin()->first.value_range());
          for (auto it = init_list.begin(); it != init_list.end(); ++it)
            values_[it->first.index()].second = it->second;
        }
      }

      ~DiscreteRandomVariableMap()
      {
      }

      DiscreteRandomVariableMap&
      operator=(const DiscreteRandomVariableMap& right)
      {
        values_.clear();
        values_ = right.values_;
        return *this;
      }

      DiscreteRandomVariableMap&
      operator=(DiscreteRandomVariableMap && right)
      {
        values_.clear();
        values_ = right.values_;
        return *this;
      }

      DiscreteRandomVariableMap&
      operator=(std::initializer_list<value_type> right)
      {
        values_.clear();
        if (right.size() != 0)
        {
          resize(right.begin()->first.value_range());
          for (auto it = right.begin(); it != right.end(); ++it)
          values_[it->first.index()].second = it->second;
        }
        return *this;
      }

      /*
       * Iterators
       */

      iterator
      begin()
      {
        return values_.begin();
      }

      const_iterator
      begin() const
      {
        return values_.begin();
      }

      iterator
      end()
      {
        return values_.end();
      }

      const_iterator
      end() const
      {
        return values_.end();
      }

      reverse_iterator
      rbegin()
      {
        return values_.rbegin();
      }

      const_reverse_iterator
      rbegin() const
      {
        return values_.rbegin();
      }

      reverse_iterator
      rend()
      {
        return values_.rend();
      }

      const_reverse_iterator
      rend() const
      {
        return values_.rend();
      }

      const_iterator
      cbegin() const
      {
        return values_.cbegin();
      }

      const_iterator
      cend() const
      {
        return values_.cend();
      }

      const_iterator
      crbegin() const
      {
        return values_.crbegin();
      }

      const_iterator
      crend() const
      {
        return values_.crend();
      }

      /*
       * Capacity
       */

      bool
      empty() const
      {
        return values_.empty();
      }

      size_type
      size() const
      {
        return values_.size();
      }

      size_type
      max_size() const
      {
        return values_.max_size();
      }

      /*
       * Element access
       */

      T&
      operator[](const key_type& k)
      {
        resize(k.value_range());
        cpprob_check_debug(k.index() < values_.size(),"DiscreteRandomVariableMap: Element access failed due to inconsistent indices (name: " << k.name() << ", range size: " << k.value_range().size() << ", index: " << k.index() << ").");
        return values_[k.index()].second;
      }

      T&
      operator[](key_type&& k)
      {
        resize(k.value_range());
        cpprob_check_debug(k.index() < values_.size(),"DiscreteRandomVariableMap: Element access failed due to inconsistent indices (name: " << k.name() << ", range size: " << k.value_range().size() << ", index: " << k.index() << ").");
        return values_[k.index()].second;
      }

      T&
      at(const key_type& k)
      {
        return values_.at(k.index());
      }

      const T& at(const key_type& k) const
      {
        return values_.at(k.index());
      }

      /*
       * Modifiers
       */

      std::pair<iterator, bool>
      insert(const value_type& v)
      {
        resize(v.first.value_range());
        cpprob_check_debug(v.first.index() < values_.size(),"DiscreteRandomVariableMap: Element insertion failed due to inconsistent indices (name: " << v.first.name() << ", range size: " << v.first.value_range().size() << ", index: " << v.first.index() << ").");
        iterator position = values_.begin() + v.first.index();
        position->second = v.second;
        return make_pair(position, true);
      }

      template <class P>
      std::pair<iterator, bool>
      insert(P&& x)
      {
        resize(x.first.value_range());
        cpprob_check_debug(x.first.index() < values_.size(),"DiscreteRandomVariableMap: Element insertion failed due to inconsistent indices (name: " << x.first.name() << ", range size: " << x.first.value_range().size() << ", index: " << x.first.index() << ").");
        iterator position = values_.begin() + x.first.index();
        position->second = x.second;
        return make_pair(position, true);
      }

      iterator
      insert(const_iterator, const value_type& v)
      {
        resize(v.first.value_range());
        cpprob_check_debug(v.first.index() < values_.size(),"DiscreteRandomVariableMap: Element insertion failed due to inconsistent indices (name: " << v.first.name() << ", range size: " << v.first.value_range().size() << ", index: " << v.first.index() << ").");
        iterator position = values_.begin() + v.first.index();
        position->second = v.second;
        return position;
      }

      template <class P>
      iterator
      insert(const_iterator, P&& x)
      {
        resize(x.first.value_range());
        cpprob_check_debug(x.first.index() < values_.size(),"DiscreteRandomVariableMap: Element insertion failed due to inconsistent indices (name: " << x.first.name() << ", range size: " << x.first.value_range().size() << ", index: " << x.first.index() << ").");
        iterator position = values_.begin() + x.first.index();
        position->second = x.second;
        return position, true;
      }

      template <class InputIterator>
      void
      insert(InputIterator first, InputIterator last)
      {
        for (; first != last; ++first)
          insert(*first);
      }

      void
      insert(std::initializer_list<value_type> list)
      {
        for (auto it = list.begin(); it != list.end(); ++it)
          insert(*it);
      }

      iterator
      erase(const_iterator position)
      {
        return values_.begin() + (++position - values_.begin());
      }

      size_type
      erase(const key_type&)
      {
        return 1;
      }

      iterator
      erase(const_iterator first, const_iterator last)
      {
        return last;
      }

      void
      swap(DiscreteRandomVariableMap& other)
      {
        values_.swap(other.values_);
      }

      void
      clear()
      {
        values_.clear();
      }

      /*
       * Map operations
       */

      iterator
      find(const key_type& k)
      {
        if (k.index() < values_.size())
          return values_.begin() + k.index();
        else
          return values_.end();
      }

      const_iterator
      find(const key_type& k) const
      {
        if (k.index() < values_.size())
          return values_.begin() + k.index();
        else
          return values_.end();
      }

      size_type
      count(const key_type& k) const
      {
        return k.index() < values_.size() ? 1 : 0;
      }

    bool
    operator==(const DiscreteRandomVariableMap& right) const
    {
      return values_ == right.values_;
    }

    bool
    operator!=(const DiscreteRandomVariableMap& right) const
    {
      return values_ != right.values_;
    }

    private:

      Container values_;

      /**
       * @throw std::length_error
       */
      void resize(const DiscreteRandomVariable::Range& value_range)
      {
        if (values_.size() > value_range.size())
          return;

        values_.reserve(value_range.size() + 1);

        DiscreteRandomVariable new_key;
        if (values_.size() == 0)
        {
          // new_key may be value_range.end() here.
          new_key = value_range.begin();
        }
        else
        {
          new_key = values_.back().first;
          ++new_key;
        }

        for (; new_key != value_range.end(); ++new_key)
        {
          values_.push_back(value_type(new_key, mapped_type()));
        }
        values_.push_back(value_type(new_key, mapped_type()));
      }

    };

        } /* namespace cpprob */

#endif /* DISCRETERANDOMVARIABLEMAP_HPP_ */

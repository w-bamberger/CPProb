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
#include <iterator>
#include <memory>

namespace cpprob
{

  template<class ValueType, class NodeIterator>
    class Iterator_ : public std::iterator<std::bidirectional_iterator_tag,
        ValueType>
    {

      typedef Iterator_<ValueType, NodeIterator> Self_;

    public:

      Iterator_()
          : node_()
      {
      }

      Iterator_(const NodeIterator& node)
          : node_(node)
      {
      }

      Self_&
      operator++()
      {
        node_ += node_->next_offset;
        ;
        return *this;
      }

      Self_
      operator++(int)
      {
        Self_ tmp = *this;
        node_ += node_->next_offset;
        return tmp;
      }

      Self_&
      operator--()
      {
        node_ -= node_->prev_offset;
        return *this;
      }

      Self_
      operator--(int)
      {
        Self_ tmp = *this;
        node_ -= node_->prev_offset;
        return tmp;
      }

      ValueType&
      operator*() const
      {
        return node_->data;
      }

      ValueType*
      operator->() const
      {
        return &(node_->data);
      }

      NodeIterator node_;

    };

  template<class ValueType, class NodeIterator, class NonConstIterator>
    class ConstIterator_ : public std::iterator<std::bidirectional_iterator_tag,
        ValueType>
    {

      typedef ConstIterator_<ValueType, NodeIterator, NonConstIterator> Self_;

    public:

      ConstIterator_()
          : node_()
      {
      }

      ConstIterator_(const NodeIterator& node)
          : node_(node)
      {
      }

      ConstIterator_(const NonConstIterator& right)
          : node_(right.node_)
      {
      }

      Self_&
      operator++()
      {
        node_ += node_->next_offset;
        return *this;
      }

      Self_
      operator++(int)
      {
        Self_ tmp = *this;
        node_ += node_->next_offset;
        return tmp;
      }

      Self_&
      operator--()
      {
        node_ -= node_->prev_offset;
        return *this;
      }

      Self_
      operator--(int)
      {
        Self_ tmp = *this;
        node_ -= node_->prev_offset;
        return tmp;
      }

      Self_&
      operator=(const Self_& right)
      {
        this->node_ = right.node_;
        return *this;
      }

      ValueType&
      operator*() const
      {
        return node_->data;
      }

      ValueType*
      operator->() const
      {
        return &(node_->data);
      }

      NodeIterator node_;

    };

  /*
   *
   */
  template<class T>
    class DiscreteRandomVariableMap
    {

      struct Node
      {

        typedef std::pair<const DiscreteRandomVariable, T> Data;

        Node()
            : prev_offset(0), next_offset(0), data()
        {
        }

        Node(const DiscreteRandomVariable& key)
            : prev_offset(0), next_offset(0), data(Data(key, T()))
        {
        }

        bool
        operator==(const Node& other) const
        {
          return prev_offset == other.prev_offset
              && next_offset == other.next_offset && data == other.data;
        }

        Node&
        operator=(const Node& other)
        {
          prev_offset = other.prev_offset;
          next_offset = other.next_offset;
          const_cast<DiscreteRandomVariable&>(data.first) = other.data.first;
          data.second = other.data.second;
          return *this;
        }

        std::ptrdiff_t prev_offset;
        std::ptrdiff_t next_offset;
        Data data;
      };

      typedef DiscreteRandomVariableMap<T> Self;
      typedef cont::vector<Node> Container;

    public:

      typedef DiscreteRandomVariable key_type;
      typedef T mapped_type;
      typedef std::pair<const DiscreteRandomVariable, T> value_type;
      typedef DiscreteRandomVariable::ValueLess key_compare;
      typedef value_type& reference;
      typedef const value_type& const_reference;
      typedef Iterator_<value_type, typename Container::iterator> iterator;
      typedef ConstIterator_<const value_type,
          typename Container::const_iterator, iterator> const_iterator;
      typedef typename Container::size_type size_type;
      typedef typename Container::difference_type difference_type;
      typedef value_type* pointer;
      typedef const value_type* const_pointer;
      typedef std::reverse_iterator<iterator> reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      /*
       * Construct, copy and destroy
       */

      DiscreteRandomVariableMap()
          : values_(1), node_count_(0)
      {
      }

      DiscreteRandomVariableMap(const Self& other)
          : values_(other.values_), node_count_(other.node_count_)
      {
      }

      DiscreteRandomVariableMap(Self&& other)
      : values_(other.values_), node_count_(other.node_count_)
      {
      }

      template<class InputIterator>
      DiscreteRandomVariableMap(InputIterator first, InputIterator last)
      : values_(1), node_count_(0)
      {
        copy(first, last);
      }

      DiscreteRandomVariableMap(std::initializer_list<value_type> init_list)
      : values_(1), node_count_(0)
      {
        copy(init_list.begin(), init_list.end());
      }

      // Use the implicit destructor, so the implicit copy and move operations
      // are generated by the compiler.

      DiscreteRandomVariableMap&
      operator=(const DiscreteRandomVariableMap& right)
      {
        if (this == &right)
        return *this;

        values_.clear();
        values_ = right.values_;
        node_count_ = right.node_count_;
        return *this;
      }

      DiscreteRandomVariableMap&
      operator=(DiscreteRandomVariableMap && right)
      {
        if (this == &right)
        return *this;

        values_.clear();
        values_ = right.values_;
        node_count_ = right.node_count_;
        return *this;
      }

      DiscreteRandomVariableMap&
      operator=(std::initializer_list<value_type> right)
      {
        clear();
        copy(right.begin(), right.end());
        return *this;
      }

      /*
       * Iterators
       */

      iterator
      begin()
      {
        return ++iterator(values_.begin());
      }

      const_iterator
      begin() const
      {
        return ++const_iterator(values_.begin());
      }

      iterator
      end()
      {
        return values_.begin();
      }

      const_iterator
      end() const
      {
        return values_.begin();
      }

      reverse_iterator
      rbegin()
      {
        return reverse_iterator(end());
      }

      const_reverse_iterator
      rbegin() const
      {
        return const_reverse_iterator(end());
      }

      reverse_iterator
      rend()
      {
        return reverse_iterator(begin());
      }

      const_reverse_iterator
      rend() const
      {
        return const_reverse_iterator(begin());
      }

      const_iterator
      cbegin() const
      {
        return const_cast<const Self&>(*this).begin();
      }

      const_iterator
      cend() const
      {
        return const_cast<const Self&>(*this).end();
      }

      const_reverse_iterator
      crbegin() const
      {
        return const_cast<const Self&>(*this).rbegin();
      }

      const_reverse_iterator
      crend() const
      {
        return const_cast<const Self&>(*this).rend();
      }

      /*
       * Capacity
       */

      bool
      empty() const
      {
        return size() == 0;
      }

      size_type
      size() const
      {
        return node_count_;
      }

      size_type
      max_size() const
      {
        return values_.max_size() - 1;
      }

      /*
       * Element access.
       *
       * I do not offer rvalue-based index access, as rvalue assignment of
       * DiscreteRandomVariables is the same as copy assignment.
       */

      T&
      operator[](const key_type& k)
      {
        resize(k.value_range());
        return do_access(k, false);
      }

      T&
      at(const key_type& k)
      {
        if (k.value_ + 1 >= values_.size())
        cpprob_throw_out_of_range("DiscreteRandomVariableMap: key " << k <<" is not in the map.");
        return do_access(k, true);
      }

      const T&
      at(const key_type& k) const
      {
        if (k.value_ + 1 >= values_.size())
          cpprob_throw_out_of_range("DiscreteRandomVariableMap: key " << k << " is not in the map.");
        return do_access(k);
      }

      /*
       * Modifiers
       */

      std::pair<iterator, bool>
      insert(const value_type& v)
      {
        resize(v.first.value_range());
        return do_insert(v);
      }

      template <class P>
      std::pair<iterator, bool>
      insert(P&& x)
      {
        resize(x.first.value_range());
        return do_insert(x);
      }

      iterator
      insert(const_iterator, const value_type& v)
      {
        resize(v.first.value_range());
        return do_insert(v).first;
      }

      template <class P>
      iterator
      insert(const_iterator, P&& x)
      {
        resize(x.first.value_range());
        return do_insert(x).first;
      }

      template <class InputIterator>
      void
      insert(InputIterator first, InputIterator last)
      {
        copy(first, last);
      }

      void
      insert(std::initializer_list<value_type> init_list)
      {
        copy(init_list.begin(), init_list.end());
      }

      void
      swap(DiscreteRandomVariableMap& other)
      {
        std::swap(node_count_, other.node_count_);
        values_.swap(other.values_);
      }

      void
      clear()
      {
        values_.clear();
        // Ensure the base element exists.
        values_.push_back(Node());
        node_count_ = 0;
      }

      /*
       * Map operations
       */

      iterator
      find(const key_type& k)
      {
        if (k.value_ + 1 < values_.size())
        {
          typename Container::iterator key_position = values_.begin() + k.value_ + 1;
          if (is_linked(*key_position))
            return key_position;
        }
        return end();
      }

      const_iterator
      find(const key_type& k) const
      {
        if (k.value_ + 1 < values_.size())
        {
          typename Container::const_iterator key_position = values_.begin() + k.value_ + 1;
          if (is_linked(*key_position))
            return key_position;
        }
        return end();
      }

      size_type
      count(const key_type& k) const
      {
        return (find(k) == end()) ? 0 : 1;
      }

      template<class U>
      friend bool
      operator==(const DiscreteRandomVariableMap<U>&, const DiscreteRandomVariableMap<U>&);

    private:

      Container values_;
      size_type node_count_;

      template <class InputIterator>
      void
      copy(InputIterator first, InputIterator last)
      {
        if (first != last)
        {
          resize(first->first.value_range());
          for (; first != last; ++first)
          do_insert(*first);
        }
      }

      mapped_type&
      do_access(const key_type& k, bool throw_if_inexistent)
      {
        cpprob_check_debug(k.value_ + 1 < values_.size(),"DiscreteRandomVariableMap: Element access failed due to inconsistent indices (name: " << k.name() << ", range size: " << k.value_range().size() << ", index: " << k.value_ << ", container size: " << values_.size() << ").");
        auto accessed_node = values_.begin() + k.value_ + 1;

        if (! is_linked(*accessed_node))
        {
          if (throw_if_inexistent)
            cpprob_throw_out_of_range("DiscreteRandomVariableMap: Key " << k << " is not in the map.");
          else
            link_node(accessed_node);
        }

        return accessed_node->data.second;
      }

      const mapped_type&
      do_access(const key_type& k) const
      {
        cpprob_check_debug(k.value_ + 1 < values_.size(),"DiscreteRandomVariableMap: Element access failed due to inconsistent indices (name: " << k.name() << ", range size: " << k.value_range().size() << ", index: " << k.value_ << ", container size: " << values_.size() << ").");
        auto accessed_node = values_.begin() + k.value_ + 1;

        if (! is_linked(*accessed_node))
          cpprob_throw_out_of_range("DiscreteRandomVariableMap: Key " << k << " is not in the map.");

        return accessed_node->data.second;
      }

      std::pair<iterator, bool>
      do_insert(const_reference v)
      {
        cpprob_check_debug(v.first.value_ + 1 < values_.size(),"DiscreteRandomVariableMap: Element assignment failed due to inconsistent indices (name: " << v.first.name() << ", range size: " << v.first.value_range().size() << ", index: " << v.first.value_ << ").");
        typename Container::iterator accessed_node = values_.begin() + v.first.value_ + 1;

        if (! is_linked(*accessed_node))
        {
          link_node(accessed_node);
          accessed_node->data.second = v.second;
          return make_pair(accessed_node, true);
        }

        return make_pair(accessed_node, false);
      }

      bool
      is_linked(const Node& node) const
      {
        cpprob_check_debug(size() == 0 || (node.prev_offset == 0 && node.next_offset == 0) || (size() != 0 && node.prev_offset != 0 && node.next_offset != 0), "DiscreteRandomVariableMap: Found an inconsistent data structure (prev: " << node.prev_offset << ", next: " << node.next_offset << ")");
        return (node.prev_offset != 0);
      }

      void
      link_node(typename Container::iterator accessed_node)
      {
        auto base = values_.begin();
        auto prev = accessed_node;
        while (!is_linked(*prev) && prev != base)
          --prev;

        auto next = prev + prev->next_offset;

        accessed_node->next_offset = next - accessed_node;
        next->prev_offset = accessed_node->next_offset;

        accessed_node->prev_offset = accessed_node - prev;
        prev->next_offset = accessed_node->prev_offset;

        ++node_count_;
      }

      /**
       * @throw std::length_error
       */
      void resize(const DiscreteRandomVariable::Range& value_range)
      {
        /*
         * Test if resizing is necessary.
         */
        if (values_.size() > value_range.size() + 1)
        return;

        /*
         * Allocate the memory.
         */
        values_.reserve(value_range.size() + 2);

        /*
         * Ensure the invisible base element exists.
         */
        cpprob_check_debug(values_.size() != 0, "DiscreteRandomVariableMap: The base element is missing in the map (inconsistent data structure).");

        /*
         * Extend the vectors to contain all elements
         * of the variable's value range.
         */
        DiscreteRandomVariable new_key;
        if (values_.size() == 1)
        {
          // new_key may be value_range.end() here.
          new_key = value_range.begin();
        }
        else
        {
          new_key = (--values_.end())->data.first;
          ++new_key;
        }
        for (; new_key != value_range.end(); ++new_key)
        {
          values_.push_back(Node(new_key));
        }
        // The vector must also contain value_range.end().
        values_.push_back(Node(new_key));
      }

    };

  template<class ValueType, class NodeIterator, class NonConstIterator>
    inline bool
    operator==(
        const ConstIterator_<ValueType, NodeIterator, NonConstIterator>& x
        , const ConstIterator_<ValueType, NodeIterator, NonConstIterator>& y)
    {
      return x.node_ == y.node_;
    }

  template<class ConstValueType, class ConstNodeIterator, class NonConstIterator>
    inline bool
    operator==(
        const NonConstIterator& x,
        const ConstIterator_<ConstValueType, ConstNodeIterator, NonConstIterator>& y)
    {
      return x.node_ == y.node_;
    }

  template<class ValueType, class NodeIterator>
    inline bool
    operator==(const Iterator_<ValueType, NodeIterator>& x
        , const Iterator_<ValueType, NodeIterator>& y)
    {
      return x.node_ == y.node_;
    }

  template<class ValueType, class NodeIterator, class NonConstIterator>
    inline bool
    operator!=(
        const ConstIterator_<ValueType, NodeIterator, NonConstIterator>& x
        , const ConstIterator_<ValueType, NodeIterator, NonConstIterator>& y)
    {
      return x.node_ != y.node_;
    }

  template<class ConstValueType, class ConstNodeIterator, class NonConstIterator>
    inline bool
    operator!=(
        const NonConstIterator& x,
        const ConstIterator_<ConstValueType, ConstNodeIterator, NonConstIterator>& y)
    {
      return x.node_ != y.node_;
    }

  template<class ValueType, class NodeIterator>
    inline bool
    operator!=(const Iterator_<ValueType, NodeIterator>& x
        , const Iterator_<ValueType, NodeIterator>& y)
    {
      return x.node_ != y.node_;
    }

  template<class T>
    inline void
    swap(DiscreteRandomVariableMap<T>& x, DiscreteRandomVariableMap<T>& y)
    {
      x.swap(y);
    }

  template<class T>
    inline bool
    operator==(const DiscreteRandomVariableMap<T>& x,
        const DiscreteRandomVariableMap<T>& y)
    {
      return x.values_ == y.values_;
    }

  template<class T>
    inline bool
    operator!=(const DiscreteRandomVariableMap<T>& x,
        const DiscreteRandomVariableMap<T>& y)
    {
      return !(x == y);
    }

} /* namespace cpprob */

#endif /* DISCRETERANDOMVARIABLEMAP_HPP_ */

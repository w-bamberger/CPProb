/*
 * ConstantNode.hpp
 *
 *  Created on: 31.08.2011
 *      Author: wbam
 */

#ifndef CONSTANTNODE_HPP_
#define CONSTANTNODE_HPP_

#include "DiscreteNode.hpp"

namespace cpprob
{

  /*
   *
   */
  template<class T>
    class ConstantNode
    {
    public:

      ConstantNode(const T& value)
          : value_(value)
      {
      }

      ~ConstantNode()
      {
      }

      template<class N>
        void
        add_child(N& child)
        {
        }

      float
      at_references() const
      {
        return 1.0;
      }

      bool
      is_evidence() const
      {
        return true;
      }

      T&
      value()
      {
        return value_;
      }

      const T&
      value() const
      {
        return value_;
      }

      void
      value(const T& value)
      {
        value_ = value;
      }

    private:

      T value_;

    };

  template<>
  class ConstantNode<DiscreteRandomVariable> : public DiscreteNode
  {

  public:

    ConstantNode(const DiscreteRandomVariable& v)
        : DiscreteNode(v)
    {
    }

    float
    at_references() const
    {
      return 1.0;
    }

    bool
    is_evidence() const
    {
      return true;
    }

  };

  template<class T>
    inline std::ostream&
    operator<<(std::ostream& os, const ConstantNode<T>& node)
    {
      return os << "Constant node " << node.value().name() << " with value\n"
          << "  " << node.value() << "\n";
    }

} /* namespace cpprob */

#endif /* CONSTANTNODE_HPP_ */

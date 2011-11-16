/*
 * ConstantNode.hpp
 *
 *  Created on: 31.08.2011
 *      Author: wbam
 */

#ifndef CONSTANTNODE_HPP_
#define CONSTANTNODE_HPP_

#include "CategoricalNode.hpp"
#include "DirichletProcessNode.hpp"

namespace cpprob
{

  template<class T, class Child>
    class ConstantNode;

  typedef ConstantNode<DirichletProcessParameters, DirichletProcessNode> ConstantDirichletProcessParametersNode;
  typedef ConstantNode<DiscreteRandomVariable, ConditionalCategoricalNode> ConstantDiscreteRandomVariableNode;
  typedef ConstantNode<RandomConditionalProbabilities,
      ConditionalCategoricalNode> ConstantRandomConditionalProbabilitiesNode;
  typedef ConstantNode<RandomProbabilities, CategoricalNode> ConstantRandomProbabilitiesNode;

  /*
   *
   */
  template<class T, class Child>
    class ConstantNode
    {

    public:

      typedef cont::RefVector<Child> Children;

      ConstantNode(const T& value)
          : value_(value)
      {
      }

      ~ConstantNode()
      {
      }

      Children&
      children()
      {
        return children_;
      }

      const Children&
      children() const
      {
        return children_;
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

      Children children_;
      T value_;

    };

  template<>
    class ConstantNode<DiscreteRandomVariable, ConditionalCategoricalNode> : public DiscreteNode
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

  template<class T, class C>
    inline std::ostream&
    operator<<(std::ostream& os, const ConstantNode<T, C>& node)
    {
      return os << "Constant node " << node.value().name() << " with value\n"
          << "  " << node.value() << "\n";
    }

} /* namespace cpprob */

#endif /* CONSTANTNODE_HPP_ */

/*
 * NodeUtils.hpp
 *
 *  Created on: 31.08.2011
 *      Author: wbam
 */

#ifndef NODEUTILS_HPP_
#define NODEUTILS_HPP_

#include "ConstantNode.hpp"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <sstream>
#include <typeinfo>

namespace cpprob
{

  class DiscreteRandomVariable;

  template<class Visitor>
    class ApplyVisitorDelayed
    {

    public:

      typedef typename Visitor::result_type result_type;

      explicit
      ApplyVisitorDelayed(const Visitor& visitor)
          : visitor_(visitor)
      {
      }

      template<typename Visitable>
        result_type
        operator()(Visitable& visitable)
        {
          return boost::apply_visitor(visitor_, visitable);
        }

      template<typename Visitable1, typename Visitable2>
        result_type
        operator()(Visitable1& visitable1, Visitable2& visitable2)
        {
          return boost::apply_visitor(visitor_, visitable1, visitable2);
        }

    private:

      Visitor visitor_;

    };

  class CompareNodeName : public boost::static_visitor<bool>
  {

  public:

    CompareNodeName(const std::string& name)
        : name_(name)
    {
    }

    template<class T>
      bool
      operator()(const T& node) const
      {
        return name_ == node.value().name();
      }

  private:

    std::string name_;

  };

  class DiscreteValueOfNode : public boost::static_visitor<
      DiscreteRandomVariable*>
  {

  public:

    DiscreteRandomVariable*
    operator()(CategoricalNode& node) const
    {
      return &node.value();
    }

    DiscreteRandomVariable*
    operator()(ConditionalCategoricalNode& node) const
    {
      return &node.value();
    }

    DiscreteRandomVariable*
    operator()(ConstantDiscreteRandomVariableNode& node) const
    {
      return &node.value();
    }

    template<class N>
      DiscreteRandomVariable*
      operator()(N&) const
      {
        return 0;
      }

  };

  class InitSamplingOfNode : public boost::static_visitor<>
  {

  public:

    template<class V, class C>
      void
      operator()(ConstantNode<V, C>) const
      {
      }

    template<class N>
      void
      operator()(N& node) const
      {
        if (!node.is_evidence())
          node.init_sampling();
      }

  };

  class NameOfNode : public boost::static_visitor<std::string&>
  {

  public:

    template<class N>
      const std::string&
      operator()(const N& node) const
      {
        return node.value().name();
      }

  };

  class NodeIsEvidence : public boost::static_visitor<bool>
  {

  public:

    template<class T>
      bool
      operator()(const T& node) const
      {
        return node.is_evidence();
      }

  };

  class ProbabilityOfNode : public boost::static_visitor<float>
  {

  public:

    float
    operator()(const CategoricalNode& node) const
    {
      return node.at_references();
    }

    float
    operator()(const ConditionalCategoricalNode& node) const
    {
      return node.at_references();
    }

    template<class T>
      float
      operator()(const T&) const
      {
        cpprob_throw_network_error(
            "ProbabilityOfNode: Cannot request a probability from a node of type " //
            << typeid(T).name() << ".");
      }

    template<class V, class C>
      float
      operator()(const ConstantNode<V, C>&) const
      {
        return 1.0;
      }

  };

  class SampleNode : public boost::static_visitor<>
  {

  public:

    template<class V, class C>
      void
      operator()(ConstantNode<V, C>&) const
      {
      }

    template<class N>
      void
      operator()(N& node) const
      {
        if (!node.is_evidence())
          node.sample();
      }

  };

  class StreamOutPointerValueToString : public boost::static_visitor<std::string>
  {

  public:

    template<class T>
      std::string
      operator()(const T* ptr) const
      {
        std::ostringstream oss;
        oss << *ptr;
        return oss.str();
      }

  };

  template<class Visitor>
    inline ApplyVisitorDelayed<Visitor>
    make_apply_visitor_delayed(const Visitor& visitor)
    {
      return ApplyVisitorDelayed<Visitor>(visitor);
    }

  inline ApplyVisitorDelayed<CompareNodeName>
  make_delayed_compare_node_name(const std::string& name)
  {
    return ApplyVisitorDelayed<CompareNodeName>(CompareNodeName(name));
  }

}

#endif /* NODEUTILS_HPP_ */

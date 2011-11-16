/*
 * DirichletProcessParameters.hpp
 *
 *  Created on: 29.09.2011
 *      Author: wbam
 */

#ifndef DIRICHLETPROCESSPARAMETERS_HPP_
#define DIRICHLETPROCESSPARAMETERS_HPP_

#include "DiscreteRandomVariableMap.hpp"
#include "cont/RefVector.hpp"

namespace DirichletProcessTest
{

  class Parameters;
  class Node;

}

namespace cpprob
{

  class ConditionalCategoricalNode;
  class ConditionalDirichletNode;
  class RandomInteger;

  /*
   *
   */
  class DirichletProcessParameters
  {


  public:

    typedef DiscreteRandomVariableMap<std::size_t> ComponentCounters;
    typedef cont::RefVector<ConditionalDirichletNode> ManagedNodes;

    DirichletProcessParameters(const std::string& name, float concentration,
        std::initializer_list<ConditionalDirichletNode*> managed_nodes);

    template<class NodeList>
      DirichletProcessParameters(const std::string& name, float concentration,
          NodeList& managed_nodes)
          : concentration_(concentration), managed_nodes_(managed_nodes.begin(),
              managed_nodes.end()), name_(name)
      {
      }

    float
    concentration() const
    {
      return concentration_;
    }

    const ComponentCounters&
    component_counters() const
    {
      return component_counters_;
    }

    const ManagedNodes&
    managed_nodes() const
    {
      return managed_nodes_;
    }

    ManagedNodes&
    managed_nodes()
    {
      return managed_nodes_;
    }

    const std::string&
    name() const
    {
      return name_;
    }

  private:

    friend class DirichletProcessNode;
    friend class ::DirichletProcessTest::Parameters;
    friend class ::DirichletProcessTest::Node;
    typedef cont::RefVector<ConditionalCategoricalNode> Children;
    typedef cont::map<DiscreteRandomVariable, Children,
        DiscreteRandomVariable::NameLess> ChildrenOfComponent;

    ComponentCounters component_counters_;
    float concentration_;
    ManagedNodes managed_nodes_;
    std::string name_;

    DiscreteRandomVariable
    create_component(const Children& children_of_component);

    void
    extend_managed_node(ConditionalDirichletNode& node,
        const DiscreteRandomVariable::Range& old_range,
        const DiscreteRandomVariable::Range& new_range,
        const ChildrenOfComponent& children_of_component);

    DiscreteRandomVariable
    next_component(const Children& children_of_component);

    DiscreteRandomVariable
    init_from_prior();

    friend std::ostream&
    operator<<(std::ostream& os, const DirichletProcessParameters& parameters);

    float
    prior_probability_of_managed_node(const ConditionalDirichletNode& node,
        ComponentCounters counters) const;

    float
    prior_probability_of_managed_nodes(
        const Children& children_of_component) const;

    void
    sample_managed_node(ConditionalDirichletNode& node,
        const DiscreteRandomVariable& component,
        DiscreteRandomVariableMap<Children>& children_of_node);

    void
    sample_managed_nodes(const DiscreteRandomVariable& component,
        const Children& children_of_component);

  };

} /* namespace cpprob */

#endif /* DIRICHLETPROCESSPARAMETERS_HPP_ */

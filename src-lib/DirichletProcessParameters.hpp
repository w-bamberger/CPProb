/*
 * DirichletProcessParameters.hpp
 *
 *  Created on: 29.09.2011
 *      Author: wbam
 */

#ifndef DIRICHLETPROCESSPARAMETERS_HPP_
#define DIRICHLETPROCESSPARAMETERS_HPP_

#include "DiscreteRandomVariable.hpp"
#include "cont/list.hpp"
#include <boost/range/adaptor/indirected.hpp>
#include <string>

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

    typedef cont::list<ConditionalDirichletNode*> ManagedNodes;

  public:

    typedef cont::map<DiscreteRandomVariable, std::size_t> ComponentCounters;
    typedef boost::indirected_range<const ManagedNodes> ConstManagedNodeRange;
    typedef boost::indirected_range<ManagedNodes> ManagedNodeRange;

    template<class NodeList>
      DirichletProcessParameters(const std::string& name, float concentration,
          NodeList& managed_nodes)
          : concentration_(concentration), managed_nodes_(managed_nodes.begin(),
              managed_nodes.end()), name_(name)
      {
      }

    ~DirichletProcessParameters();

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

    ConstManagedNodeRange
    managed_nodes() const;

    ManagedNodeRange
    managed_nodes();

    const std::string&
    name() const
    {
      return name_;
    }

  private:

    friend class DirichletProcessNode;
    friend class ::DirichletProcessTest::Parameters;
    friend class ::DirichletProcessTest::Node;
    typedef cont::list<ConditionalCategoricalNode*> Children;
    typedef cont::list<const ConditionalCategoricalNode*> ConstChildren;

    ComponentCounters component_counters_;
    float concentration_;
    ManagedNodes managed_nodes_;
    std::string name_;

    DiscreteRandomVariable
    create_component(const Children& children_of_component);

    void
    extend_managed_node(
        ConditionalDirichletNode& node,
        const DiscreteRandomVariable::Range& old_range,
        const DiscreteRandomVariable::Range& new_range,
        const cont::map<DiscreteRandomVariable, ConstChildren>& children_of_component);

    DiscreteRandomVariable
    next_component(const Children& children_of_component);

    DiscreteRandomVariable
    init_from_prior();

    friend std::ostream&
    operator<<(std::ostream& os, const DirichletProcessParameters& parameters);

    void
    sample_managed_node(ConditionalDirichletNode& node,
        const DiscreteRandomVariable& component,
        cont::map<DiscreteRandomVariable, ConstChildren>& children_of_node);

    void
    sample_managed_nodes(const DiscreteRandomVariable& component,
        const Children& children_of_component);

  };

} /* namespace cpprob */

#endif /* DIRICHLETPROCESSPARAMETERS_HPP_ */

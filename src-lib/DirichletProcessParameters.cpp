/*
 * DirichletProcessParameters.cpp
 *
 *  Created on: 29.09.2011
 *      Author: wbam
 */

#include "DirichletProcessParameters.hpp"
#include "ConditionalDirichletNode.hpp"
#include "DiscreteJointRandomVariable.hpp"
#include "RandomInteger.hpp"
#include "cont/vector.hpp"
#include <ostream>

using namespace std;

namespace cpprob
{

  std::ostream&
  operator<<(std::ostream& os, const DirichletProcessParameters& parameters)
  {
    return os << parameters.name() << ":" << parameters.concentration();
  }

#ifndef WITHOUT_INITIALIZER_LIST
  DirichletProcessParameters::DirichletProcessParameters(
      const std::string& name, float concentration,
      std::initializer_list<ConditionalDirichletNode*> managed_nodes)
      : component_counters_(), component_name_(name), concentration_(
          concentration), managed_nodes_(managed_nodes.begin(),
          managed_nodes.end()), parameters_name_(name + "Parameters")
  {
  }
#endif

  DiscreteRandomVariable
  DirichletProcessParameters::create_component(
      const Children& children_of_component)
  {
    auto old_range = RandomInteger(component_name_ + "_old",
        component_counters_.size(), 0).value_range();
    // Creates the component and also sets the size in the characteristics
    // structure to the new value.
    RandomInteger new_component(component_name_, component_counters_.size() + 1,
        component_counters_.size());
    auto new_range = new_component.value_range();
    component_counters_[new_component] = 0;

    for (auto node = managed_nodes_.begin(); node != managed_nodes_.end();
        ++node)
    {
      ChildrenOfComponent children_of_node;
      for (auto child = children_of_component.begin();
          child != children_of_component.end(); ++child)
      {
        if (&child->probabilities() == &node->value())
          children_of_node[new_component].push_back(*child);
      }
      extend_managed_node(*node, old_range, new_range, children_of_node);
    }
    return new_component;
  }

  void
  DirichletProcessParameters::extend_managed_node(
      ConditionalDirichletNode& node,
      const DiscreteRandomVariable::Range& old_range,
      const DiscreteRandomVariable::Range&,
      const ChildrenOfComponent& children_of_node)
  {
    /**
     * @todo Refactor around children_of_node.
     * It is a map of which only one value is taken!
     *
     * Third parameter is unused!
     */

    /* Check requirements. */
    cpprob_check_debug(
        node.children().begin() != node.children().end(),
        "DirichletProcessParameters: Cannot extend a ConditionalDirichletNode without children.");

    /* Construct a joint reference to the extended condition
     * (with the new component), condition_refs, and a joint reference with the
     * old condition set, tmp_old_condition_vars. */
    auto node_children_it = node.children().begin();
    const DiscreteRandomReferences& condition_refs =
        node_children_it->condition();

    RandomInteger old_self_condition(old_range.end());
    DiscreteRandomReferences old_condition_refs;
    if (old_range.size() != 0)
    {
      --old_self_condition;
      old_condition_refs.insert(old_self_condition);
    }
    const RandomInteger last_old_self_condition(old_self_condition);

    DiscreteJointRandomVariable::iterator new_self_condition;
    DiscreteJointRandomVariable new_condition;

    for (auto var = condition_refs.begin(); var != condition_refs.end(); ++var)
    {
      cpprob_check_debug(
          var->value_range().size() != 0,
          "DirichletProcessParameters: Cannot make a joint condition with the empty variable " << var->name() << ".");

      /* Insert all variable with their last value. So I can step backwards in
       * the end. The variable of the own node is already the extended variable.
       * So it only comes in the container of the new condition. The container
       * of the old condition already contains a variable for the own node
       * with the old value range. It has been inserted above. */
      if (var->name() == component_name_)
      {
        // This condition variable comes only in the new condition container.
        // It is the extended variable.
        auto self_init_value = var->value_range().end();
        --self_init_value;
        new_self_condition = new_condition.insert(self_init_value).first;
      }
      else
      {
        auto init_value = var->value_range().end();
        --init_value;
        auto insert_result = new_condition.insert(init_value);
        cpprob_check_debug(
            insert_result.second,
            "DirichletProcessParameters: Could not insert the variable " << init_value << " in the condition set " << new_condition << ".");
        auto condition_var = insert_result.first;
        old_condition_refs.insert(*condition_var);
      }
    }
    const DiscreteRandomVariable tmp_condition_begin =
        new_condition.value_range().begin();
    DiscreteRandomVariable::ValueLess value_less;

    while (1)
    {
      /* Update or insert the probabilities of the current condition. */
      if (old_range.size() == 0
          || value_less(last_old_self_condition, *new_self_condition))
      {
        auto found_children = children_of_node.find(*new_self_condition);
        if (found_children == children_of_node.end())
          node.sample(new_condition, Children());
        else
          node.sample(new_condition, found_children->second);
      }
      else
      {
        size_t self_value = RandomInteger(*new_self_condition).observation();
        old_self_condition.observation(self_value);
        size_t old_value =
            RandomInteger(old_condition_refs.joint_value()).observation();
        RandomInteger old_condition(new_condition);
        old_condition.observation(old_value);
        RandomConditionalProbabilities& probability_table = node.value();
        /* @todo Better with swap? */
        probability_table[new_condition] = probability_table[old_condition];
      }

      if (new_condition == tmp_condition_begin)
        break;
      else
        --new_condition;
    }
  }

  DiscreteRandomVariable
  DirichletProcessParameters::next_component(
      const Children& children_of_component)
  {
    for (auto it = component_counters_.begin(); it != component_counters_.end();
        ++it)
    {
      if (it->second == 0)
      {
        sample_managed_nodes(it->first, children_of_component);
        return it->first;
      }
    }

    return create_component(children_of_component);
  }

  float
  DirichletProcessParameters::prior_probability_of_managed_nodes(
      const Children& children_of_component) const
  {
    float p = 1.0;
    for (auto node = managed_nodes_.begin(); node != managed_nodes_.end();
        ++node)
    {
      ComponentCounters child_counters;
      for (auto child = children_of_component.begin();
          child != children_of_component.end(); ++child)
      {
        if (&child->probabilities() == &node->value())
          child_counters[child->value()] += 1;
      }
      p *= prior_probability_of_managed_node(*node, child_counters);
    }
    return p;
  }

  float
  DirichletProcessParameters::prior_probability_of_managed_node(
      const ConditionalDirichletNode& node, ComponentCounters counters) const
  {
    float sum_parameters = 0.0;
    size_t sum_counters = 0;
    float nominator = 1.0;
    for (auto parameter = node.parameters().begin();
        parameter != node.parameters().end(); ++parameter)
    {
      float p = parameter->second;
      sum_parameters += p;
      size_t counter = counters[parameter->first];
      sum_counters += counter;
      for (; counter != 0; --counter)
      {
        nominator *= p;
        p += 1.0;
      }
    }

    float denominator = 1.0;
    for (; sum_counters != 0; --sum_counters)
    {
      denominator *= sum_parameters;
      sum_parameters += 1;
    }

    return nominator / denominator;
  }

  void
  DirichletProcessParameters::sample_managed_node(
      ConditionalDirichletNode& node, const DiscreteRandomVariable& component,
      DiscreteRandomVariableMap<Children>& children_of_node)
  {
    /* Check requirements. */
    cpprob_check_debug(
        node.children().begin() != node.children().end(),
        "DirichletProcessParameters: Cannot extend a ConditionalDirichletNode without children.");

    const DiscreteRandomReferences& condition_refs =
        node.children().begin()->condition();
    DiscreteJointRandomVariable condition_var;
    DiscreteJointRandomVariable::iterator condition_self_it;

    for (auto var = condition_refs.begin(); var != condition_refs.end(); ++var)
    {
      cpprob_check_debug(
          var->value_range().size() != 0,
          "DirichletProcessParameters: Cannot make a joint condition with the empty variable " << var->name() << ".");

      /* Insert all variable with their last value. So I can step backwards in
       * the end. The variable of the own node is already the extended variable.
       * So it only comes in the container of the new condition. The container
       * of the old condition already contains a variable for the own node
       * with the old value range. It has been inserted above. */
      auto insert_result = condition_var.insert(var->value_range().begin());
      cpprob_check_debug(
          insert_result.second,
          "DirichletProcessParameters: Could not insert the variable " << var->value_range().begin() << " in the condition set " << condition_var << ".");
      if (var->name() == component_name_)
        condition_self_it = insert_result.first;
    }

    for (; condition_var != condition_var.value_range().end(); ++condition_var)
    {
      if (*condition_self_it == component)
        node.sample(condition_var, children_of_node[condition_var]);
    }
  }

  void
  DirichletProcessParameters::sample_managed_nodes(
      const DiscreteRandomVariable& component,
      const Children& children_of_component)
  {
    for (auto node = managed_nodes_.begin(); node != managed_nodes_.end();
        ++node)
    {
      DiscreteRandomVariableMap<Children> children_of_node;
      for (auto child = children_of_component.begin();
          child != children_of_component.end(); ++child)
      {
        if (&child->probabilities() == &node->value())
          children_of_node[component].push_back(*child);
      }
      sample_managed_node(*node, component, children_of_node);
    }
  }

} /* namespace cpprob */

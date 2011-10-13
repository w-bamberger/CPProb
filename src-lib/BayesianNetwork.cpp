/*
 * BayesianNetwork.cpp
 *
 *  Created on: 07.07.2011
 *      Author: wbam
 */

#include "BayesianNetwork.hpp"
#include "Error.hpp"
#include "IoUtils.hpp"
#include "NodeUtils.hpp"
#include <algorithm>
#include <ostream>
#include <tuple>
#include <typeinfo>

using namespace boost;
using namespace std;

namespace cpprob
{

  class BayesianNetwork::CopyNode : public static_visitor<>
  {

    typedef variant<CategoricalNode*, ConditionalCategoricalNode*,
        ConditionalDirichletNode*, ConstantNode<DirichletProcessParameters>*,
        ConstantNode<DiscreteRandomVariable>*,
        ConstantNode<RandomConditionalProbabilities>*,
        ConstantNode<RandomProbabilities>*, DirichletNode*,
        DirichletProcessNode*> NodePointer;
    typedef variant<const CategoricalNode*, const ConditionalCategoricalNode*,
        const ConditionalDirichletNode*,
        const ConstantNode<DirichletProcessParameters>*,
        const ConstantNode<DiscreteRandomVariable>*,
        const ConstantNode<RandomConditionalProbabilities>*,
        const ConstantNode<RandomProbabilities>*, const DirichletNode*,
        const DirichletProcessNode*> ConstNodePointer;
    typedef variant<const DirichletProcessParameters*,
        const DiscreteRandomVariable*, const RandomConditionalProbabilities*,
        const RandomProbabilities*> ValuePointer;
    typedef map<ConstNodePointer, NodePointer> NodeNodeTable;
    typedef map<ValuePointer, NodePointer> ValueNodeTable;

  public:

    explicit
    CopyNode(BayesianNetwork& new_network)
        : new_network_(new_network)
    {
    }

    void
    operator()(const CategoricalNode& old_node)
    {
      ValuePointer old_probabilities = &old_node.probabilities();
      ValueNodeTable::iterator new_probabilities_node = value_node_table.find(
          old_probabilities);
      if (new_probabilities_node == value_node_table.end())
        cpprob_throw_logic_error(
            "BayesianNetwork: Could not copy the network because of an inconsistent structure.");

      if (DirichletNode** new_dirichlet = get<DirichletNode*>(
          &new_probabilities_node->second))
      {
        CategoricalNode& new_node = new_network_.add_categorical(
            old_node.value(), **new_dirichlet);
        new_node.is_evidence(old_node.is_evidence());
        node_node_table[&old_node] = &new_node;
        value_node_table[&old_node.value()] = &new_node;
      }
      else if (ConstantNode < RandomProbabilities > **new_constant = get<
      ConstantNode<RandomProbabilities>*>(
          &new_probabilities_node->second))
      {
        CategoricalNode& new_node = new_network_.add_categorical(
            old_node.value(), **new_constant);
        new_node.is_evidence(old_node.is_evidence());
        node_node_table[&old_node] = &new_node;
        value_node_table[&old_node.value()] = &new_node;
      }
    }

    void
    operator()(const ConditionalCategoricalNode& old_node)
    {
      ValuePointer old_probabilities = &old_node.probabilities();
      ValueNodeTable::iterator new_probabilities_node = value_node_table.find(
          old_probabilities);
      if (new_probabilities_node == value_node_table.end())
        cpprob_throw_logic_error(
            "BayesianNetwork: Could not copy the network because of an inconsistent structure.");

      list<DiscreteNode*> new_condition_nodes;
      for (DiscreteRandomReferences::const_iterator c =
          old_node.condition().begin(); c != old_node.condition().end(); ++c)
      {
        ValueNodeTable::iterator new_condition_node = value_node_table.find(
            &(*c));
        if (new_condition_node == value_node_table.end())
          cpprob_throw_logic_error(
              "BayesianNetwork: Could not copy the network because of an inconsistent structure.");

        if (CategoricalNode** new_categorical = get<CategoricalNode*>(&new_condition_node->second))
          new_condition_nodes.push_back(*new_categorical);

        else if (ConditionalCategoricalNode** new_conditional_categorical = get<ConditionalCategoricalNode*>(&new_condition_node->second))
          new_condition_nodes.push_back(*new_conditional_categorical);

        else
          cpprob_throw_logic_error(
              "BayesianNetwork: Could not copy the network because of an inconsistent structure.");
      }

      if (ConditionalDirichletNode** new_dirichlet = get<ConditionalDirichletNode*>(&new_probabilities_node->second))
      {
        ConditionalCategoricalNode& new_node =
            new_network_.add_conditional_categorical(old_node.value(),
                new_condition_nodes, **new_dirichlet);
        new_node.is_evidence(old_node.is_evidence());
        node_node_table[&old_node] = &new_node;
        value_node_table[&old_node.value()] = &new_node;
      }
      else if (ConstantNode<RandomConditionalProbabilities>** new_constant = get<ConstantNode<RandomConditionalProbabilities>*>(&new_probabilities_node->second))
      {
        ConditionalCategoricalNode& new_node =
            new_network_.add_conditional_categorical(old_node.value(),
                new_condition_nodes, **new_constant);
        new_node.is_evidence(old_node.is_evidence());
        node_node_table[&old_node] = &new_node;
        value_node_table[&old_node.value()] = &new_node;
      }
    }

    void
    operator()(const ConditionalDirichletNode& old_node)
    {
      ConditionalDirichletNode& new_node =
          new_network_.add_conditional_dirichlet(old_node.value(),
              old_node.parameters().begin()->second);
      new_node.is_evidence(old_node.is_evidence());
      node_node_table[&old_node] = &new_node;
      value_node_table[&old_node.value()] = &new_node;
    }

    void
    operator()(const DirichletNode& old_node)
    {
      DirichletNode& new_node = new_network_.add_dirichlet(old_node.value(),
          old_node.parameters().begin()->second);
      new_node.is_evidence(old_node.is_evidence());
      node_node_table[&old_node] = &new_node;
      value_node_table[&old_node.value()] = &new_node;
    }

    void
    operator()(const ConstantNode<DirichletProcessParameters>& old_node)
    {
      typedef ConstantNode<DirichletProcessParameters> NodeType;
      const auto& old_value = old_node.value();

      // Translate the Dirichlet nodes managed by the Dirichlet process.
      cont::list<ConditionalDirichletNode*> managed_nodes;
      auto old_managed_nodes = old_value.managed_nodes();
      for (auto node_it = old_managed_nodes.begin();
          node_it != old_managed_nodes.end(); ++node_it)
      {
        ConditionalDirichletNode* old_managed_node = &(*node_it);
        ConditionalDirichletNode* new_managed_node = get<
            ConditionalDirichletNode*>(node_node_table.at(old_managed_node));
        managed_nodes.push_back(new_managed_node);
      }

      // Create the new node for the Dirichlet process parameters.
      NodeType& new_node = new_network_.add_dirichlet_process_parameters(
          old_value.name(), old_value.concentration(), managed_nodes);
      node_node_table[&old_node] = &new_node;
      value_node_table[&old_value] = &new_node;
    }

    void
    operator()(const DirichletProcessNode& old_node)
    {
      auto* new_parameters_node =
          get<ConstantNode<DirichletProcessParameters>*>(
              value_node_table.at(&old_node.parameters()));

      auto& new_node = new_network_.add_dirichlet_process(*new_parameters_node);
      node_node_table[&old_node] = &new_node;
      value_node_table[&old_node.value()] = &new_node;
    }

    template<class V>
      void
      operator()(const ConstantNode<V>& old_node)
      {
        ConstantNode<V>& new_node = new_network_.add_constant(old_node.value());
        node_node_table[&old_node] = &new_node;
        value_node_table[&old_node.value()] = &new_node;
      }

  private:

    BayesianNetwork& new_network_;
    NodeNodeTable node_node_table;
    ValueNodeTable value_node_table;

  };

  class BayesianNetwork::LearnParameters : public static_visitor<>
  {

  public:

    /**
     * Learns a parameter of type RandomProbabilities that is Dirichlet
     * distributed. So this method learns the maximum a posteriori probabilities
     * given the data and the Dirichlet prior. To get the maximum likelihood
     * probabilities, just set the Dirichlet parameters to 0.
     *
     * @param node the parameter node to learn
     * @throw Any #learn() catches all exceptions and maps them appropriately
     *     for its interface.
     */
    void
    operator()(DirichletNode& node) const
    {
      if (node.is_evidence())
        return;

      /* Clear the target variable. I use it for counters and normalize it
       * in the end. */
      RandomProbabilities& probabilities = node.value();
      probabilities.clear();

      /* Initialize the counters with the parameter values of the Dirichlet
       * prior values. Without this pre-initialization, the remaining
       * algorithm performs just maximum likelihood learning. */
      copy(node.parameters().begin(), node.parameters().end(),
          inserter(probabilities, probabilities.begin()));

      /* Add the likelihood to the counters. */
      DirichletNode::ChildRange children = node.children();
      for (DirichletNode::ChildRange::iterator child = children.begin();
          child != children.end(); ++child)
      {
        if (child->is_evidence())
          probabilities[child->value()] += 1.0;
      }

      /* Normalize the counters to probabilities. */
      probabilities.normalize();
    }

    /**
     * Learns a parameter of type RandomConditionalProbabilities that is
     * Dirichlet distributed. So this method learns the maximum a posteriori
     * probabilities given the data and the Dirichlet prior. To get the maximum
     * likelihood probabilities, just set the Dirichlet parameters to 0.
     *
     * @param node the parameter node to learn
     * @throw Any #learn() catches all exceptions and maps them appropriately
     *     for its interface.
     */
    void
    operator()(ConditionalDirichletNode& node) const
    {
      if (node.is_evidence())
        return;

      /* Check requirements */
      cpprob_check_debug(
          node.children().begin() != node.children().end(),
          "BayesianNetwork: Cannot learn the conditional Dirichlet node without children (node name: " + node.value().name() + ").");

      /* Clear the target variable. I use it for counters and normalize it
       * in the end. */
      RandomConditionalProbabilities& probabilities = node.value();
      probabilities.clear();

      /* Initialize the counters with the values of the Dirichlet prior
       * distribution. If the prior values are zero, the remaining algorithm
       * performs just maximum likelihood learning. I need the range of the
       * condition variable first to set up the table of the counters. The
       * parameter set of the Dirichlet prior contains only one Dirichlet
       * distribution that is equal for all condition values. */
      DiscreteRandomVariable::Range condition_range =
          node.children().begin()->condition().joint_value().value_range();
      for (auto condition = condition_range.begin();
          condition != condition_range.end(); ++condition)
      {
        RandomProbabilities& prob_set = probabilities[condition];
        copy(node.parameters().begin(), node.parameters().end(),
            inserter(prob_set, prob_set.begin()));
      }

      /* Add the likelihood to the counters. */
      ConditionalDirichletNode::ChildRange children = node.children();
      for (ConditionalDirichletNode::ChildRange::iterator child =
          children.begin(); child != children.end(); ++child)
      {
        if (child->is_evidence())
        {
          DiscreteRandomVariable& child_value = child->value();
          DiscreteRandomVariable child_condition =
              child->condition().joint_value();
          probabilities[child_condition][child_value] += 1.0;
        }
      }

      /* Normalize the counters to probabilities. */
      probabilities.normalize();
    }

    template<class N>
      void
      operator()(N&) const
      {
      }

  };

  ostream&
  operator<<(ostream& os, const BayesianNetwork& bn)
  {
    NodeIsEvidence is_evidence_visitor;
    bool is_large_network = bn.size() > 10;

    if (is_large_network)
      os << "Non-evidence vertices of the Bayesian network:\n";
    else
      os << "Vertices of the Bayesian network:\n";

    for (BayesianNetwork::const_iterator n = bn.begin(); n != bn.end(); ++n)
    {
      bool node_is_evidence = apply_visitor(is_evidence_visitor, *n);
      if (node_is_evidence && is_large_network)
        continue;

      os << *n;
    }

    return os;
  }

  BayesianNetwork::BayesianNetwork()
  {
  }

  BayesianNetwork::BayesianNetwork(const BayesianNetwork& other_hbn)
  {
    for_each(other_hbn.begin(), other_hbn.end(),
        make_apply_visitor_delayed(CopyNode(*this)));
  }

  BayesianNetwork::~BayesianNetwork()
  {
  }

  CategoricalNode&
  BayesianNetwork::add_categorical(const DiscreteRandomVariable& value)
  {
    ConstantNode<RandomProbabilities>& parameter_node = add_constant(
        RandomProbabilities(value));
    return add_categorical(value, parameter_node);
  }

  ConditionalCategoricalNode&
  BayesianNetwork::add_conditional_categorical(
      const DiscreteRandomVariable& value,
      const cont::list<DiscreteNode*>& condition_nodes)
  {
    DiscreteRandomReferences condition;
    cont::list<DiscreteNode*>::const_iterator n = condition_nodes.begin();
    for (; n != condition_nodes.end(); ++n)
      condition.insert((*n)->value());

    ConstantNode<RandomConditionalProbabilities>& parameter_node = add_constant(
        RandomConditionalProbabilities(value, condition.joint_value()));
    return add_conditional_categorical(value, condition_nodes, parameter_node);
  }

  ConditionalDirichletNode&
  BayesianNetwork::add_conditional_dirichlet(
      const RandomConditionalProbabilities& value, float alpha)
  {
    iterator new_node = vertices_.insert(end(),
        ConditionalDirichletNode(value, alpha));
    return get<ConditionalDirichletNode>(*new_node);
  }

  DirichletNode&
  BayesianNetwork::add_dirichlet(const RandomProbabilities& value, float alpha)
  {
    iterator new_node = vertices_.insert(end(), DirichletNode(value, alpha));
    return get<DirichletNode>(*new_node);
  }

  DirichletProcessNode&
  BayesianNetwork::add_dirichlet_process(
      ConstantNode<DirichletProcessParameters>& parent)
  {
    iterator new_node = vertices_.insert(end(),
        DirichletProcessNode(parent.value()));
    return get<DirichletProcessNode>(*new_node);
  }

  CategoricalDistribution
  BayesianNetwork::enumerate(CategoricalNode& X_v)
  {
    CategoricalDistribution X_distribution;
    // Save the current state of the evidence flag to restore it in the end.
    bool temp_evidence_flag = X_v.is_evidence();

    try
    {
      X_v.is_evidence(true);
      enumerate_impl(X_distribution, X_v.value());
      X_v.is_evidence(temp_evidence_flag);
    }
    catch (...)
    {
      X_v.is_evidence(temp_evidence_flag);
      throw;
    }

    return X_distribution;
  }

  CategoricalDistribution
  BayesianNetwork::enumerate(ConditionalCategoricalNode& X_v)
  {
    CategoricalDistribution X_distribution;
    // Save the current state of the evidence flag to restore it in the end.
    bool temp_evidence_flag = X_v.is_evidence();

    try
    {
      X_v.is_evidence(true);
      enumerate_impl(X_distribution, X_v.value());
      X_v.is_evidence(temp_evidence_flag);
    }
    catch (...)
    {
      X_v.is_evidence(temp_evidence_flag);
      throw;
    }

    return X_distribution;
  }

  float
  BayesianNetwork::enumerate_all(iterator current, iterator end)
  {
    if (current == end)
      return 1.0;

    float conditional_probability = 0.0;
    ProbabilityOfNode probability_visitor;
    iterator next = current;
    ++next;

    if (apply_visitor(NodeIsEvidence(), *current))
    {
      conditional_probability = apply_visitor(probability_visitor, *current);
      return conditional_probability * enumerate_all(next, end);
    }
    else
    {
      DiscreteRandomVariable* x = apply_visitor(DiscreteValueOfNode(),
          *current);
      if (x == 0)
        return enumerate_all(next, end);

      DiscreteRandomVariable::Range X_range = x->value_range();
      double probability_sum = 0.0;

      for (*x = X_range.begin(); *x != X_range.end(); ++(*x))
      {
        conditional_probability = apply_visitor(probability_visitor, *current);
        probability_sum += conditional_probability * enumerate_all(next, end);
      }
      return probability_sum;
    }
  }

  void
  BayesianNetwork::enumerate_impl(CategoricalDistribution& X_distribution,
      DiscreteRandomVariable& x)
  {
    // The vertices must be sorted in topological order for this algorithm.

    try
    {
      DiscreteRandomVariable::Range X_range = x.value_range();

      for (x = X_range.begin(); x != X_range.end(); ++x)
        X_distribution[x] = enumerate_all(begin(), end());

      X_distribution.normalize();
    }
    catch (const NetworkError& e)
    {
      throw;
    }
    catch (const std::bad_alloc& e)
    {
      throw;
    }
    catch (const std::exception& e)
    {
      cpprob_throw_network_error(
          "BayesianNetwork: Could not enumerate the network. " //
          << e.what());
    }
  }

  CategoricalNode&
  BayesianNetwork::insert_categorical(const DiscreteRandomVariable& value,
      RandomProbabilities& parameters)
  {
    iterator new_node = vertices_.insert(end(),
        CategoricalNode(value, parameters));
    return get<CategoricalNode>(*new_node);
  }

  ConditionalCategoricalNode&
  BayesianNetwork::insert_conditional_categorical(
      const DiscreteRandomVariable& value,
      const DiscreteRandomReferences& condition,
      RandomConditionalProbabilities& parameters)
  {
    iterator new_node = vertices_.insert(end(),
        ConditionalCategoricalNode(value, condition, parameters));
    return get<ConditionalCategoricalNode>(*new_node);
  }

  CategoricalDistribution
  BayesianNetwork::sample(const DiscreteNode& X,
      unsigned int burn_in_iterations, unsigned int collect_iterations)
  {
    CategoricalDistribution X_distribution;
    const DiscreteRandomVariable& x = X.value();

    for_each(begin(), end(), make_apply_visitor_delayed(InitSamplingOfNode()));

    for (unsigned int iteration = 0; iteration < burn_in_iterations;
        iteration++)
      for_each(begin(), end(), make_apply_visitor_delayed(SampleNode()));

    // Sample the distribution
    for (unsigned int iteration = 0; iteration < collect_iterations;
        iteration++)
    {
      SampleNode sample_visitor;

      for (iterator vertex_it = begin(); vertex_it != end(); ++vertex_it)
      {
        apply_visitor(sample_visitor, *vertex_it);
        X_distribution[x] += 1.0f;
      }
    }

    X_distribution.normalize();
    return X_distribution;
  }

  void
  BayesianNetwork::learn()
  {
    for_each(begin(), end(), make_apply_visitor_delayed(LearnParameters()));
  }

}

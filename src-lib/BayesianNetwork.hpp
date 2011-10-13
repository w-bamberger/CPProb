/*
 * BayesianNetwork.hpp
 *
 *  Created on: 07.07.2011
 *      Author: wbam
 */

#ifndef BAYESIANNETWORK_HPP_
#define BAYESIANNETWORK_HPP_

#include "CategoricalDistribution.hpp"
#include "CategoricalNode.hpp"
#include "ConditionalCategoricalNode.hpp"
#include "ConditionalDirichletNode.hpp"
#include "ConstantNode.hpp"
#include "DirichletNode.hpp"
#include "DirichletProcessNode.hpp"
#include "NodeUtils.hpp"
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>

namespace cpprob
{

  class BayesianNetwork
  {

  public:

    typedef boost::variant<CategoricalNode, ConditionalCategoricalNode,
        ConditionalDirichletNode, ConstantNode<DirichletProcessParameters>,
        ConstantNode<DiscreteRandomVariable>,
        ConstantNode<RandomConditionalProbabilities>,
        ConstantNode<RandomProbabilities>, DirichletNode, DirichletProcessNode> Node;

  private:

    typedef cont::list<Node> NodeList;

  public:

    typedef NodeList::iterator iterator;
    typedef NodeList::const_iterator const_iterator;

    BayesianNetwork();

    BayesianNetwork(const BayesianNetwork& other_hbn);

    ~BayesianNetwork();

    CategoricalNode&
    add_categorical(const DiscreteRandomVariable& value);

    template<class N>
      CategoricalNode&
      add_categorical(const DiscreteRandomVariable& value, N& parameter_node)
      {
        CategoricalNode& new_node = insert_categorical(value,
            parameter_node.value());
        parameter_node.add_child(new_node);
        return new_node;
      }

    ConditionalCategoricalNode&
    add_conditional_categorical(const DiscreteRandomVariable& value,
        const cont::list<DiscreteNode*>& condition_nodes);

    template<class N>
      ConditionalCategoricalNode&
      add_conditional_categorical(const DiscreteRandomVariable& value,
          const cont::list<DiscreteNode*>& condition_nodes, N& parameter_node)
      {
        DiscreteRandomReferences condition;
        cont::list<DiscreteNode*>::const_iterator n = condition_nodes.begin();
        for (; n != condition_nodes.end(); ++n)
          condition.insert((*n)->value());

        ConditionalCategoricalNode& new_node = insert_conditional_categorical(
            value, condition, parameter_node.value());

        parameter_node.add_child(new_node);
        for (n = condition_nodes.begin(); n != condition_nodes.end(); ++n)
          (*n)->add_child(new_node);

        return new_node;
      }

    template<class N>
      ConditionalCategoricalNode&
      add_conditional_categorical(const DiscreteRandomVariable& value,
          const cont::list<DirichletProcessNode*> condition_nodes,
          N& parameter_node)
      {
        DiscreteRandomReferences condition;
        for (auto n = condition_nodes.begin(); n != condition_nodes.end(); ++n)
          condition.insert((*n)->value());

        ConditionalCategoricalNode& new_node = insert_conditional_categorical(
            value, condition, parameter_node.value());

        parameter_node.add_child(new_node);
        for (auto n = condition_nodes.begin(); n != condition_nodes.end(); ++n)
          (*n)->add_child(new_node);

        return new_node;
      }

    ConditionalDirichletNode&
    add_conditional_dirichlet(const RandomConditionalProbabilities& value,
        float alpha);

    template<class V>
      ConstantNode<V>&
      add_constant(const V& value)
      {
        iterator new_node = vertices_.insert(end(), ConstantNode<V>(value));
        return boost::get<ConstantNode<V> >(*new_node);
      }

    DirichletNode&
    add_dirichlet(const RandomProbabilities& value, float alpha);

    DirichletProcessNode&
    add_dirichlet_process(ConstantNode<DirichletProcessParameters>& parent);

    template<class NodeList>
      ConstantNode<DirichletProcessParameters>&
      add_dirichlet_process_parameters(const std::string& name,
          float concentration, const NodeList& managed_nodes)
      {
        typedef ConstantNode<DirichletProcessParameters> NodeType;
        iterator new_node = vertices_.insert(
            end(),
            NodeType(
                DirichletProcessParameters(name, concentration,
                    managed_nodes)));
        return boost::get<NodeType>(*new_node);
      }

    iterator
    begin()
    {
      return vertices_.begin();
    }

    const_iterator
    begin() const
    {
      return vertices_.begin();
    }

    iterator
    end()
    {
      return vertices_.end();
    }

    const_iterator
    end() const
    {
      return vertices_.end();
    }

    /**
     * Computes the probability distribution of the given vertex by enumeration.
     * This algorithm only works with discrete distributions
     * (CategoricalDistribution and ConditionalCategoricalDistribution). To
     * allow continuous variables for fixed parents vertices, you must associate
     * them with the DeltaDistribution.
     *
     * For such a discrete Bayesian network, the joint probability can be
     * computed by enumerating all possible states. This is what this algorithm
     * does.
     *
     * @par Requires:
     * - The network consists only of random variables of the type
     *   DiscreteRandomVariable or its sub-classes and of distributions of the
     *   types CategoricalDistribution and ConditionalCategoricalDistribution.
     *   The only exception is an evidence vertex without parents. Such a
     *   vertex may use the DeltaDistribution.
     *
     * @par Ensures:
     * - The distributions and the evidence flags are not modified, even in
     *   case of an exception.
     * - The random variable values of evidence vertices are not modified.
     *   All other random variables are used by the algorithm and thus are
     *   modified during execution.
     *
     * @param X_n node whose distribution should be computed
     * @return the unconditional distribution of the variable of the vertex X_v
     * @throw NetworkError The network does not conform to the requirements of
     *     this algorithm.
     * @throw std::bad_alloc Failed to allocate temporary memory.
     */
    CategoricalDistribution
    enumerate(CategoricalNode& X_n);

    CategoricalDistribution
    enumerate(ConditionalCategoricalNode& X_n);

    template<class N>
      N&
      find(const std::string& name)
      {
        return boost::get<N>(
            *std::find_if(begin(), end(), make_delayed_compare_node_name(name)));
      }

    template<class N>
      const N&
      find(const std::string& name) const
      {
        return boost::get<N>(
            *std::find_if(begin(), end(), make_delayed_compare_node_name(name)));
      }

    friend std::ostream&
    operator<<(std::ostream& os, const BayesianNetwork& hbn);

    CategoricalDistribution
    sample(const DiscreteNode& X, unsigned int burn_in_iterations,
        unsigned int collect_iterations);

    /**
     * Fills the values of parameter vertices from the data in the network.
     * This method looks for the non-evidence vertices with random variables of
     * the types RandomProbabilities or RandomConditionalProbabilities. It then
     * tries to learn these probabilities from the values of the child vertices
     * that are evidence. The method is maximum a posteriori learning if the
     * parameters are associated with a DirichletDistribution or a
     * CondDirichletDistribution as their prior. If such a prior is not found,
     * maximum likelihood learning is used.
     *
     * @par Requires:
     * - The vertices that should be learned must contain a random variable
     *   of the class RandomProbabilities or RandomConditionalProbabilities.
     *   In addition, the evidence flag of those vertices may not be set.
     * - The evidence to learn from must be represented as child  of the
     *   parameter vertices. (This is how Bayesian parameter learning is usually
     *   represented in a Bayesian network.) These vertices must have the
     *   evidence flag set. Otherwise they are ignored.
     * - The evidence vertices for a RandomProbabilities parameter vertex must
     *   contain a random variable of the class DiscreteRandomVariable. The
     *   evidence vertices for a RandomConditionalProbabilities parameter vertex
     *   must contain a random variable of the class DiscreteRandomVariable and
     *   a distribution of the class ConditionalCategoricalDistribution.
     *
     * @par Ensures:
     * - Every parameter vertex that can be learned (see requirements) contains
     *   the probabilities learned by maximum likelihood or maximum a posteriori
     *   learning.
     * - All other values in the network remain unchanged.
     *
     * @throw NetworkError The network violates the requirements given above.
     */
    void
    learn();

    std::size_t
    size() const
    {
      return vertices_.size();
    }

  private:

    class CopyNode;
    class LearnParameters;

    NodeList vertices_;

    /**
     * Computes recursively the joint probability of the network defined by
     * the vertices between current and end. The algorithm enumerates all
     * possibilities. It is a help function for #enumerate().
     *
     * This method may throw any exception. They are all caught in #enumerate()
     * and transformed in appropriate exceptions.
     *
     * @param current For this vertex, the probability is computed.
     * @param end If current == end, the recursion stops with 1.0.
     * @return the joint probability of the network
     * @throw Any #enumerate() catches all exceptions and maps them
     *     appropriately for its interface.
     */
    float
    enumerate_all(iterator current, iterator end);

    void
    enumerate_impl(CategoricalDistribution& X_distribution,
        DiscreteRandomVariable& x);

    CategoricalNode&
    insert_categorical(const DiscreteRandomVariable& value,
        RandomProbabilities& parameters);

    ConditionalCategoricalNode&
    insert_conditional_categorical(const DiscreteRandomVariable& value,
        const DiscreteRandomReferences& condition_nodes,
        RandomConditionalProbabilities& parameters);

  };

}

#endif /* BAYESIANNETWORK_HPP_ */

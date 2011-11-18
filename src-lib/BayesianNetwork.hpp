/*
 * BayesianNetwork.hpp
 *
 *  Created on: 07.07.2011
 *      Author: wbam
 */

#ifndef BAYESIANNETWORK_HPP_
#define BAYESIANNETWORK_HPP_

#include "ConditionalDirichletNode.hpp"
#include "DirichletNode.hpp"
#include "NodeUtils.hpp"
#include "cont/list.hpp"
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>

namespace cpprob
{

  class BayesianNetwork
  {

  public:

    typedef boost::variant<CategoricalNode, ConditionalCategoricalNode,
        ConditionalDirichletNode, ConstantDirichletProcessParametersNode,
        ConstantDiscreteRandomVariableNode,
        ConstantRandomConditionalProbabilitiesNode,
        ConstantRandomProbabilitiesNode, DirichletNode, DirichletProcessNode> Node;

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
        parameter_node.children().push_back(new_node);
        return new_node;
      }

    ConditionalCategoricalNode&
    add_conditional_categorical(const DiscreteRandomVariable& value,
        const cont::RefVector<DiscreteNode>& condition_nodes);

    template<class N>
      ConditionalCategoricalNode&
      add_conditional_categorical(const DiscreteRandomVariable& value,
          const cont::RefVector<DiscreteNode>& condition_nodes,
          N& parameter_node)
      {
        DiscreteRandomReferences condition;
        for (auto n = condition_nodes.begin(); n != condition_nodes.end(); ++n)
          condition.insert(n->value());

        auto& new_node = insert_conditional_categorical(value, condition,
            parameter_node.value());

        parameter_node.children().push_back(new_node);
        for (auto n = condition_nodes.begin(); n != condition_nodes.end(); ++n)
          n->children().push_back(new_node);

        return new_node;
      }

    ConditionalDirichletNode&
    add_conditional_dirichlet(const RandomConditionalProbabilities& value,
        float alpha);

    ConstantDirichletProcessParametersNode&
    add_constant(const DirichletProcessParameters& value);

    ConstantDiscreteRandomVariableNode&
    add_constant(const DiscreteRandomVariable& value);

    ConstantRandomConditionalProbabilitiesNode&
    add_constant(const RandomConditionalProbabilities& value);

    ConstantRandomProbabilitiesNode&
    add_constant(const RandomProbabilities& value);

    DirichletNode&
    add_dirichlet(const RandomProbabilities& value, float alpha);

    DirichletProcessNode&
    add_dirichlet_process(ConstantDirichletProcessParametersNode& parent);

    ConstantDirichletProcessParametersNode&
    add_dirichlet_process_parameters(const std::string& name,
        float concentration,
        const std::initializer_list<ConditionalDirichletNode*>& managed_nodes);

    template<class NodeList>
      ConstantDirichletProcessParametersNode&
      add_dirichlet_process_parameters(const std::string& name,
          float concentration, const NodeList& managed_nodes)
      {
        iterator new_node = vertices_.insert(
            end(),
            ConstantDirichletProcessParametersNode(
                DirichletProcessParameters(name, concentration,
                    managed_nodes)));
        return boost::get<ConstantDirichletProcessParametersNode>(*new_node);
      }

    template<class N>
      N&
      at(const std::string& name)
      {
        auto find_it = std::find_if(begin(), end(),
            make_delayed_compare_node_name(name));
        if (find_it == end())
          cpprob_throw_out_of_range(
              "BayesianNetwork: Could not find the node " + name + ".");
        return boost::get<N>(*find_it);
      }

    template<class N>
      const N&
      at(const std::string& name) const
      {
        auto find_it = std::find_if(begin(), end(),
            make_delayed_compare_node_name(name));
        if (find_it == end())
          cpprob_throw_out_of_range(
              "BayesianNetwork: Could not find the node " + name + ".");
        return boost::get<N>(*find_it);
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

    /**
     * Removes the given node from the network. This method erases exactly
     * the single node that is at the address given by the argument reference.
     * It does not consider the name or values of the node. In addition, the
     * method removes all child references of its parent nodes.
     *
     * If the referenced node is found, it is removed. In this case, this
     * method returns 1. If the node is not found, 0 is returned.
     *
     * This method is very slow. It must walk through the whole network to
     * find all the references to the node. To save memory and speed up
     * computations, there are no data structures for finding these references.
     * Erasing is considered to be very unimportant and rarely used.
     *
     * The node to erase may not have children any more. If it has some though,
     * this method throws an @c std::invalid_argument exception.
     */
    template<class Node>
      std::size_t
      erase(const Node& node)
      {
        cpprob_check_debug(&node != 0,
            "BayesianNetwork: Cannot erase node at address 0.");

        if (node.children().size() != 0)
          cpprob_throw_invalid_argument(
              "BayesianNetwork: Cannot erase a node with children (node " << node.value().name() << ").");

        size_t erase_count = 0;
        EraseHelper erase_helper(&node);
        for (auto n = begin(); n != end();)
        {
          if (boost::apply_visitor(erase_helper, *n))
          {
            n = vertices_.erase(n);
            ++erase_count;
          }
          else
          {
            ++n;
          }
        }
        return erase_count;
      }

    std::size_t
    size() const
    {
      return vertices_.size();
    }

  private:

    class CopyNode;
    class LearnParameters;

    class EraseHelper : public boost::static_visitor<bool>
    {

    public:

      EraseHelper(const void* node_address)
          : erase_node_address_(node_address)
      {
      }

      template<class Node>
        bool
        operator()(Node& node)
        {
          if (&node == erase_node_address_)
            return true;

          auto& children = node.children();
          for (auto c = children.begin(); c != children.end();)
          {
            if (&(*c) == erase_node_address_)
              c = children.erase(c);
            else
              ++c;
          }
          return false;
        }

    private:

      const void* erase_node_address_;

    };

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

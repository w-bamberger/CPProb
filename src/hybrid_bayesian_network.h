/*
 * hybrid_bayesian_network.h
 *
 *  Created on: 07.07.2011
 *      Author: wbam
 */

#ifndef HYBRID_BAYESIAN_NETWORK_H_
#define HYBRID_BAYESIAN_NETWORK_H_

#include "categorical_distribution.h"
#include "conditional_categorical_distribution.h"
#include "cond_dirichlet_distribution.h"
#include "count_distribution.h"
#include "degenerate_distribution.h"
#include <boost/variant/variant.hpp>

#ifdef VANET_DEBUG_MODE
#include <debug/list>
namespace vanet
{
  namespace cont
  {
    using namespace __gnu_debug;
  }
}
#else
#include <list>
namespace vanet
{
  namespace cont
  {
    using namespace std;
  }
}
#endif

namespace vanet
{

  class BoundProbabilityDistribution;
  class CategoricalDistribution;
  class RandomConditionalProbabilities;

  class HybridBayesianNetwork
  {

  public:

    class Vertex;
    typedef cont::list<Vertex> VertexList;
    typedef VertexList::iterator iterator;
    typedef VertexList::const_iterator const_iterator;
    typedef cont::list<iterator> VertexReferences;

    class Vertex
    {

    public:

      typedef HybridBayesianNetwork::VertexReferences VertexReferences;
      typedef boost::variant< //
          CategoricalDistribution, //
          ConditionalCategoricalDistribution, //
          CondDirichletDistribution, //
          DegenerateDistribution, //
          DirichletDistribution> //
      DistributionVariant;

      DistributionVariant&
      distribution()
      {
        return distribution_;
      }

      const DistributionVariant&
      distribution() const
      {
        return distribution_;
      }

      RandomVariable&
      random_variable()
      {
        return random_variable_;
      }

      const RandomVariable&
      random_variable() const
      {
        return random_variable_;
      }

      bool
      value_is_evidence() const
      {
        return value_is_evidence_;
      }

      void
      value_is_evidence(bool e)
      {
        value_is_evidence_ = e;
      }

      VertexReferences&
      children()
      {
        return children_;
      }

      const VertexReferences&
      children() const
      {
        return children_;
      }

      VertexReferences&
      parents()
      {
        return parents_;
      }

      const VertexReferences&
      parents() const
      {
        return parents_;
      }

    private:

      friend class HybridBayesianNetwork;

      DistributionVariant distribution_;
      RandomVariable& random_variable_;
      bool value_is_evidence_;

      VertexReferences children_;
      VertexReferences parents_;

      Vertex(RandomVariable& var, const DistributionVariant& distribution) :
          distribution_(distribution), random_variable_(var), value_is_evidence_(
              false)

      {
      }

    };

    class NameOfVertex;
    class CompareVertexName;

    friend std::ostream&
    operator<<(std::ostream& os, const HybridBayesianNetwork& hbn);

    HybridBayesianNetwork();

    /// @todo Copying does not work correctly yet.
    HybridBayesianNetwork(const HybridBayesianNetwork& other_hbn);

    ~HybridBayesianNetwork();

    iterator
    add_categorical(const DiscreteRandomVariable& var);

    iterator
    add_categorical(const DiscreteRandomVariable& var, iterator params_vertex);

    iterator
    add_conditional_categorical(const DiscreteRandomVariable& var,
        const VertexReferences& parents);

    iterator
    add_conditional_categorical(const DiscreteRandomVariable& var,
        const VertexReferences& parents, iterator params_vertex);

    iterator
    add_conditional_dirichlet(const RandomConditionalProbabilities& var,
        float alpha);

    iterator
    add_constant(const RandomConditionalProbabilities& var);

    iterator
    add_constant(const RandomProbabilities& var);

    iterator
    begin();

    const_iterator
    begin() const;

    iterator
    end();

    const_iterator
    end() const;

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
     * @param X_v vertex whose distribution should be computed
     * @return the unconditional distribution of the variable of the vertex X_v
     * @throw NetworkError The network does not conform to the requirements of
     *     this algorithm.
     * @throw std::bad_alloc Failed to allocate temporary memory.
     */
    CategoricalDistribution
    enumerate(iterator X_v);

    CategoricalDistribution
    gibbs_sampling(const iterator& X_it, unsigned int burn_in_iterations,
        unsigned int collect_iterations);

  private:

    class CategoricalMarkovBlanket;
    class DirichletMarkovBlanket;
    class MarkovBlanket;
    class ProbabilityVisitor;

    VertexList vertices_;

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
     * @throw Any
     */
    float
    enumerate_all(iterator current, iterator end);

  };

  class HybridBayesianNetwork::NameOfVertex : public std::unary_function<
      const HybridBayesianNetwork::Vertex&, std::string&>
  {

  public:

    const std::string&
    operator()(const HybridBayesianNetwork::Vertex& vertex) const
    {
      return vertex.random_variable().name();
    }

  };

  class HybridBayesianNetwork::CompareVertexName : public std::unary_function<
      bool, Vertex>
  {

  public:

    CompareVertexName(const std::string& name) :
        name_(name)
    {
    }

    bool
    operator()(const Vertex& vertex)
    {
      return name_ == vertex.random_variable_.name();
    }

  private:

    std::string name_;

  };

}

#endif /* HYBRID_BAYESIAN_NETWORK_H_ */

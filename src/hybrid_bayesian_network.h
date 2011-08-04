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
        distribution_(distribution), random_variable_(var),
            value_is_evidence_(false)

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

    CategoricalDistribution
    gibbs_sampling(const iterator& X_it, unsigned int burn_in_iterations,
        unsigned int collect_iterations);

  private:

    class MarkovBlanket;
    class CategoricalMarkovBlanket;
    class DirichletMarkovBlanket;

    VertexList vertices_;

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

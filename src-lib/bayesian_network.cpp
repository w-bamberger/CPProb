/*
 * bayesian_network.cpp
 *
 *  Created on: 07.07.2011
 *      Author: wbam
 */

#include "error.hpp"
#include "bayesian_network.hpp"
#include "io_utils.hpp"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <tr1/tuple>
#include <algorithm>
#include <ostream>
#include <typeinfo>
#ifdef VANET_DEBUG_MODE
#include <debug/vector>
#else
#include <vector>
#endif

using namespace std;
using namespace std::tr1;

namespace cpprob
{

  class BayesianNetwork::MarkovBlanket
  {

  public:

    virtual void
    sample() = 0;

  };

  class BayesianNetwork::CategoricalMarkovBlanket : public BayesianNetwork::MarkovBlanket
  {

    typedef cont::vector<BoundProbabilityDistribution*> Distributions;

  public:

    CategoricalMarkovBlanket(const iterator& X_v) :
        sampling_variate_(random_number_engine, CategoricalDistribution())
    {
      x_ = dynamic_cast<DiscreteRandomVariable*>(&X_v->random_variable());if (x_ == 0)
        cpprob_throw_network_error(
            "CategoricalMarkovBlanket is assigned to non-discrete random variable "
            << X_v->random_variable().name() << " of type "
            << typeid(X_v->random_variable()).name());

      distributions_.reserve(1 + X_v->children_.size());
      distributions_.push_back(
          to_discrete_probability_distribution(X_v->distribution_));
      for (VertexReferences::iterator c = X_v->children_.begin(); c
          != X_v->children_.end(); ++c)
        {
          distributions_.push_back(
              to_discrete_probability_distribution((*c)->distribution()));
        }
    } 

      virtual void
      sample()
      {
        CategoricalDistribution& sampling_distribution =
        sampling_variate_.distribution();
        sampling_distribution.clear();

        DiscreteRandomVariable::Range x_range = x_->value_range();
        for (*x_ = x_range.begin(); *x_ != x_range.end(); ++(*x_))
        {
          float p = 1;
          for (Distributions::const_iterator d = distributions_.begin(); d
              != distributions_.end(); ++d)
          {
            p *= (*d)->at_references();
          }

          sampling_distribution[*x_] = p;
        }

        sampling_distribution.normalize();
        *x_ = sampling_variate_();
      }

    private:

      Distributions distributions_;
      DiscreteRandomVariable* x_;
      variate_generator<RandomNumberEngine&, CategoricalDistribution>
      sampling_variate_;

      BoundProbabilityDistribution*
      to_discrete_probability_distribution(
          Vertex::DistributionVariant& distribution)
      {
        BoundProbabilityDistribution* d = 0;
        if ((d = get<CategoricalDistribution> (&distribution)) != 0)
        {
        }
        else if ((d = get<ConditionalCategoricalDistribution> (&distribution))
            != 0)
        {
        }
        else if ((d = get<DegenerateDistribution> (&distribution)) != 0)
        {
        }
        else
        cpprob_throw_runtime_error(
            "CategoricalMarkovBlanket: Cannot use distribution of type "
            << distribution.type().name() << ".");

        return d;
      }

    };

  class BayesianNetwork::DirichletMarkovBlanket : public BayesianNetwork::MarkovBlanket
  {

  public:

    DirichletMarkovBlanket(const BayesianNetwork::iterator& X_v) :
        X_v_(X_v)
    {

    }

    virtual void
    sample()
    {
      try
      {
        CondDirichletDistribution sample_distribution = boost::get<
            CondDirichletDistribution>(X_v_->distribution());
        for (BayesianNetwork::VertexReferences::const_iterator c =
            X_v_->children().begin(); c != X_v_->children().end(); ++c)
        {
          const DiscreteRandomVariable & c_var =
              dynamic_cast<DiscreteRandomVariable&>((*c)->random_variable());const DiscreteRandomVariable
              & c_condition = get<ConditionalCategoricalDistribution> (
                  (*c)->distribution()).referenced_condition();
              sample_distribution.at(c_condition).parameters()[c_var] += 1.0;
            }
        RandomConditionalProbabilities & x =
            dynamic_cast<RandomConditionalProbabilities&>(X_v_->random_variable());variate_generator
        <RandomNumberEngine&, CondDirichletDistribution> sampling_variate_(
            random_number_engine, sample_distribution);
        x = sampling_variate_();
      }
      catch (const bad_cast& bc)
      {
        cpprob_throw_network_error(
            "DirichletMarkovBlanket: Can only process "
            "random variables of type RandomConditionalProbabilities. " << bc.what());
      }
      catch (const boost::bad_get& bg)
      {
        cpprob_throw_network_error(
            "DirichletMarkovBlanket: Cannot use distribution of type " << X_v_->distribution().type().name() << "; it should be of type CondDirichletDistribution.");
      }
    }

  private:

    BayesianNetwork::iterator X_v_;

  };

  class BayesianNetwork::ProbabilityVisitor : public boost::static_visitor<float>
  {

  public:

    template<class T>
      float
      operator()(const T& distribution) const
      {
        cpprob_throw_network_error(
            "Cannot request a probability from a distribution of type " //
            << typeid(T).name() << ".");
      }

  };

  template<>
    float
    BayesianNetwork::ProbabilityVisitor::operator()(
        const CategoricalDistribution& distribution) const
    {
      return distribution.at_references();
    }

  template<>
    float
    BayesianNetwork::ProbabilityVisitor::operator()(
        const ConditionalCategoricalDistribution& distribution) const
    {
      return distribution.at_references();
    }

  template<>
    float
    BayesianNetwork::ProbabilityVisitor::operator()(
        const DegenerateDistribution& distribution) const
    {
      return distribution.at_references();
    }

  ostream&
  operator<<(ostream& os, const BayesianNetwork& bn)
  {
    bool is_large_network = bn.size() > 10;

    if (is_large_network)
      os << "Non-evidence vertices of the Bayesian network:\n";
    else
      os << "Vertices of the Bayesian network:\n";

    for (BayesianNetwork::const_iterator vIt = bn.begin(); vIt != bn.end();
        ++vIt)
        {
      if (vIt->value_is_evidence() && is_large_network)
        continue;

      os << "  Vertex of variable " << vIt->random_variable().name();
      if (vIt->value_is_evidence())
        os << " with value " << vIt->random_variable() << "\n";

      else
        os << " with no evidence\n";

      os << "    Parents:";
      for (BayesianNetwork::VertexReferences::const_iterator p =
          vIt->parents().begin(); p != vIt->parents().end(); ++p)
        os << " " << (*p)->random_variable().name();
      os << "\n";

      os << "    Distribution:\n";
      os << vIt->distribution();
    }
    return os;
  }

  BayesianNetwork::BayesianNetwork()
  {
  }

  BayesianNetwork::BayesianNetwork(const BayesianNetwork& other_hbn) :
      vertices_(other_hbn.vertices_)
  {
  }

  BayesianNetwork::~BayesianNetwork()
  {
    for (iterator v = begin(); v != end(); ++v)
      delete &v->random_variable();
  }

  BayesianNetwork::iterator
  BayesianNetwork::add_categorical(const DiscreteRandomVariable& var)
  {
    // Allocate the new memory
    DiscreteRandomVariable* new_var = new DiscreteRandomVariable(var);
    CategoricalDistribution new_distribution(*new_var);

    // Link everything
    return vertices_.insert(vertices_.end(), Vertex(*new_var, new_distribution));
  }

  BayesianNetwork::iterator
  BayesianNetwork::add_categorical(const DiscreteRandomVariable& var,
      iterator params_vertex)
  {
    // First, convert everything that could fail.
    RandomVariable* params_rv = &params_vertex->random_variable();
    RandomProbabilities * params = dynamic_cast<RandomProbabilities*>(params_rv);
    if (!params)
      cpprob_throw_runtime_error(
          "BayesianNetwork: Cannot connect parameters of type" //
          << typeid(*params_rv).name() << " to CategoricalDistribution.");

    // Now it is save to allocate the new memory
    DiscreteRandomVariable* new_var = new DiscreteRandomVariable(var);
    CategoricalDistribution new_distribution(*new_var, *params);

    // Finally link everything
    iterator new_vertex = vertices_.insert(vertices_.end(),
        Vertex(*new_var, new_distribution));
    new_vertex->parents().push_back(params_vertex);
    params_vertex->children().push_back(new_vertex);
    return new_vertex;
  }

  BayesianNetwork::iterator
  BayesianNetwork::add_conditional_categorical(
      const DiscreteRandomVariable& var,
      const VertexReferences& condition_vertices)
  {
    // First, convert everything that could fail.
    cont::vector<const DiscreteRandomVariable*> condition_references;
    for (VertexReferences::const_iterator c_it = condition_vertices.begin();
        c_it != condition_vertices.end(); ++c_it)
        {
      RandomVariable* c_rv = &(*c_it)->random_variable();
      if (DiscreteRandomVariable* c_drv = dynamic_cast<DiscreteRandomVariable*> (c_rv))
        condition_references.push_back(c_drv);
      else
        cpprob_throw_runtime_error(
            "BayesianNetwork: Cannot connect variable of type " //
            << typeid(*c_rv).name() << " to ConditionalCategoricalDistribution.");
    }

// Now it is save to allocate the new memory
    DiscreteRandomVariable* new_var = new DiscreteRandomVariable(var);
    ConditionalCategoricalDistribution new_distribution(*new_var,
        condition_references);

// Finally link everything
    iterator new_vertex = vertices_.insert(vertices_.end(),
        Vertex(*new_var, new_distribution));
    for (VertexReferences::const_iterator c_it = condition_vertices.begin();
        c_it != condition_vertices.end(); ++c_it)
        {
      new_vertex->parents().push_back(*c_it);
      (*c_it)->children().push_back(new_vertex);
    }

    return new_vertex;
  }

  BayesianNetwork::iterator
  BayesianNetwork::add_conditional_categorical(
      const DiscreteRandomVariable& var,
      const VertexReferences& condition_vertices, iterator params_vertex)
  {
    // First, convert everything that could fail.
    RandomVariable* params_rv = &params_vertex->random_variable();
    RandomConditionalProbabilities * params =
        dynamic_cast<RandomConditionalProbabilities*>(params_rv);
    if (!params)
      cpprob_throw_runtime_error(
          "BayesianNetwork: Cannot connect parameters of type" //
          << typeid(*params_rv).name()//
          << " to ConditionalCategoricalDistribution.");

    cont::vector<const DiscreteRandomVariable*> condition_references;
    for (VertexReferences::const_iterator c_it = condition_vertices.begin();
        c_it != condition_vertices.end(); ++c_it)
        {
      RandomVariable* c_rv = &(*c_it)->random_variable();
      if (DiscreteRandomVariable* c_drv = dynamic_cast<DiscreteRandomVariable*> (c_rv))
        condition_references.push_back(c_drv);
      else
        cpprob_throw_runtime_error(
            "BayesianNetwork: Cannot connect variable of type " //
            << typeid(*c_rv).name() << " to ConditionalCategoricalDistribution.");
    }

    // Now it is save to allocate the new memory
    DiscreteRandomVariable* new_var = new DiscreteRandomVariable(var);
    ConditionalCategoricalDistribution new_distribution(*new_var,
        condition_references, *params);

    // Finally link everything
    iterator new_vertex = vertices_.insert(vertices_.end(),
        Vertex(*new_var, new_distribution));
    new_vertex->parents().push_back(params_vertex);
    params_vertex->children().push_back(new_vertex);
    for (VertexReferences::const_iterator c_it = condition_vertices.begin();
        c_it != condition_vertices.end(); ++c_it)
        {
      new_vertex->parents().push_back(*c_it);
      (*c_it)->children().push_back(new_vertex);
    }

    return new_vertex;
  }

  BayesianNetwork::iterator
  BayesianNetwork::add_conditional_dirichlet(
      const RandomConditionalProbabilities& var, float alpha)
  {
    RandomConditionalProbabilities* new_var =
        new RandomConditionalProbabilities(var);
    CondDirichletDistribution new_distribution(*new_var, alpha);
    return vertices_.insert(vertices_.end(), Vertex(*new_var, new_distribution));
  }

  BayesianNetwork::iterator
  BayesianNetwork::add_constant(const RandomConditionalProbabilities& var)
  {
    RandomConditionalProbabilities* new_var =
        new RandomConditionalProbabilities(var);
    DegenerateDistribution new_distribution(*new_var);
    return vertices_.insert(vertices_.end(), Vertex(*new_var, new_distribution));
  }

  BayesianNetwork::iterator
  BayesianNetwork::add_constant(const RandomProbabilities& var)
  {
    RandomProbabilities* new_var = new RandomProbabilities(var);
    DegenerateDistribution new_distribution(*new_var);
    return vertices_.insert(vertices_.end(), Vertex(*new_var, new_distribution));
  }

  BayesianNetwork::iterator
  BayesianNetwork::add_dirichlet(const RandomProbabilities& var, float alpha)
  {
    RandomProbabilities* new_var = new RandomProbabilities(var);
    DirichletDistribution new_distribution(*new_var, alpha);
    return vertices_.insert(vertices_.end(), Vertex(*new_var, new_distribution));
  }

  BayesianNetwork::iterator
  BayesianNetwork::begin()
  {
    return vertices_.begin();
  }

  BayesianNetwork::const_iterator
  BayesianNetwork::begin() const
  {
    return vertices_.begin();
  }

  BayesianNetwork::iterator
  BayesianNetwork::end()
  {
    return vertices_.end();
  }

  BayesianNetwork::const_iterator
  BayesianNetwork::end() const
  {
    return vertices_.end();
  }

  CategoricalDistribution
  BayesianNetwork::enumerate(iterator X_v)
  {
    // The vertices must be sorted in topological order for this algorithm.

    CategoricalDistribution X_distribution;
    // Save the current state of the evidence flag to restore it in the end.
    bool temp_evidence_flag = X_v->value_is_evidence();

    try
    {
      DiscreteRandomVariable& x =
          dynamic_cast<DiscreteRandomVariable&>(X_v->random_variable());X_v->value_is_evidence(true);
          DiscreteRandomVariable::Range X_range = x.value_range();

          for (x = X_range.begin(); x != X_range.end(); ++x)
          X_distribution[x] = enumerate_all(begin(), end());

          X_distribution.normalize();
          X_v->value_is_evidence(temp_evidence_flag);
        }
        catch (const NetworkError& e)
        {
          X_v->value_is_evidence(temp_evidence_flag);
          throw;
        }
        catch(const std::bad_alloc& e)
        {
          X_v->value_is_evidence(temp_evidence_flag);
          throw;
        }
        catch(const std::exception& e)
        {
          X_v->value_is_evidence(temp_evidence_flag);
          cpprob_throw_network_error("BayesianNetwork: Could not enumerate the network. " //
              << e.what());
        }

    return X_distribution;
  }

  float
  BayesianNetwork::enumerate_all(iterator current, iterator end)
  {
    if (current == end)
      return 1.0;

    float conditional_probability = 0.0;
    ProbabilityVisitor probability_visitor;
    iterator next = current;
    ++next;

    if (current->value_is_evidence())
    {
      conditional_probability = boost::apply_visitor(probability_visitor,
          current->distribution());
      return conditional_probability * enumerate_all(next, end);
    }
    else
    {
      float probability_sum = 0.0;
      DiscreteRandomVariable& x =
          dynamic_cast<DiscreteRandomVariable&>(current->random_variable());DiscreteRandomVariable
      ::Range X_range = x.value_range();

      for (x = X_range.begin(); x != X_range.end(); ++x)
      {
        conditional_probability = boost::apply_visitor(probability_visitor,
            current->distribution());
        probability_sum += conditional_probability * enumerate_all(next, end);
      }
      return probability_sum;
    }
  }

  CategoricalDistribution
  BayesianNetwork::sample(const iterator& X_it, unsigned int burn_in_iterations,
      unsigned int collect_iterations)
  {
    CategoricalDistribution X_distribution;
    DiscreteRandomVariable* x =
        dynamic_cast<DiscreteRandomVariable*>(&X_it->random_variable());if
(    x == 0)
    throw runtime_error(
    "BayesianNetwork: Cannot sample vertex because of an unknown variable type.");

// Compile a list of the non-evidence variables
    typedef cont::list<pair<iterator, MarkovBlanket*> > NonEvidenceVertices;
    NonEvidenceVertices non_evidence_vertices;
    NonEvidenceVertices::iterator vertex_it;
    for (iterator v = vertices_.begin(); v != vertices_.end(); ++v)
    {
      if (v->value_is_evidence())
        continue;

      else if (get<CategoricalDistribution>(&v->distribution()) != 0)
        non_evidence_vertices.push_back(
            make_pair(v, new CategoricalMarkovBlanket(v)));

      else if (get<CondDirichletDistribution>(&v->distribution()) != 0)
        non_evidence_vertices.push_back(
            make_pair(v, new DirichletMarkovBlanket(v)));

      else if (get<ConditionalCategoricalDistribution>(&v->distribution()) != 0)
        non_evidence_vertices.push_back(
            make_pair(v, new CategoricalMarkovBlanket(v)));

      else
        throw runtime_error(
            "BayesianNetwork: Unknown distribution type for Gibbs sampling.");
    }

// Initialise non-evidence variables randomly
    NonEvidenceVertices::iterator vertices_begin_it =
        non_evidence_vertices.begin();
    NonEvidenceVertices::iterator vertices_end_it = non_evidence_vertices.end();
    for (vertex_it = vertices_begin_it; vertex_it != vertices_end_it;
        ++vertex_it)
        {
      vertex_it->first->random_variable().assign_random_value(
          random_number_engine);
    }

// Burn in sampling
    for (unsigned int iteration = 0; iteration < burn_in_iterations;
        iteration++)
        {
      for (vertex_it = vertices_begin_it; vertex_it != vertices_end_it;
          ++vertex_it)
          {
        vertex_it->second->sample();
      }
    }

// Sample the distribution
    random_number_engine.seed(2);
    for (unsigned int iteration = 0; iteration < collect_iterations;
        iteration++)
        {
      for (vertex_it = vertices_begin_it; vertex_it != vertices_end_it;
          ++vertex_it)
          {
        vertex_it->second->sample();
        X_distribution[*x] += 1.0f;
      }
    }

    X_distribution.normalize();

// Clean up
    for (vertex_it = vertices_begin_it; vertex_it != vertices_end_it;
        ++vertex_it)
      delete vertex_it->second;

    return X_distribution;
  }

  void
  BayesianNetwork::learn()
  {
    // Could be well parallelized and nicely realized with delayed visitation.

    /* I may only throw NetworkError. All other errors (like casting error)
     * are mapped to NetworkErrors with this try-catch block. */
    try
    {
      for (iterator v = begin(); v != end(); ++v)
      {
        /* Learn only non-evidence vertices with random variables of a paramter
         * type. */
        if (!v->value_is_evidence())
        {
          if (dynamic_cast<RandomProbabilities*>(&v->random_variable()))learn_probabilities(v);

          else if (dynamic_cast<RandomConditionalProbabilities*>(&v->random_variable()))
          learn_conditional_probabilities(v);
        }
      }
    }
    catch (const NetworkError&)
    {
      throw;
    }
    catch (const exception& e)
    {
      cpprob_throw_network_error(
          "BayesianNetwork: Cannot learn the provided network because of an invalid structure: "
          << e.what());
    }
  }

  void
  BayesianNetwork::learn_probabilities(iterator v)
  {
    /* Clear the target variable. I use it for counters and normalize it
     * in the end. */
    RandomProbabilities& probabilities =
        dynamic_cast<RandomProbabilities&>(v->random_variable());probabilities
        .clear();

        /* See whether a Dirichlet prior is available. If so initialize
         * the counters with the prior values. Without this pre-initialization,
         * the remaining algorithm performs just maximum likelihood learning. */
        DirichletDistribution * dirichlet_prior = get<DirichletDistribution>(
            &v->distribution());
        if (dirichlet_prior)
        {
          copy(dirichlet_prior->parameters().begin(),
              dirichlet_prior->parameters().end(),
              inserter(probabilities, probabilities.begin()));
        }

        /* Add the likelihood to the counters. */
        for (VertexReferences::iterator child = v->children().begin();
            child != v->children().end(); ++child)
        {
          if ((*child)->value_is_evidence())
          {
            DiscreteRandomVariable& child_value =
            dynamic_cast<DiscreteRandomVariable&>((*child)->random_variable());probabilities[child_value] += 1.0;
          }
        }

        /* Normalize the counters to probabilities. */
        probabilities.normalize();
      }

  void
  BayesianNetwork::learn_conditional_probabilities(iterator v)
  {
    /* Clear the target variable. I use it for counters and normalize it
     * in the end. */
    RandomConditionalProbabilities& probabilities =
        dynamic_cast<RandomConditionalProbabilities&>(v->random_variable());probabilities
        .clear();

        /* See whether a Dirichlet prior is available. If so initialize
         * the counters with the prior values. Without this pre-initialization,
         * the remaining algorithm performs just maximum likelihood learning. */
        CondDirichletDistribution* prior = get<CondDirichletDistribution>(
            &v->distribution());
        if (prior)
        {
          for (CondDirichletDistribution::const_iterator d = prior->begin();
              d != prior->end(); ++d)
          {
            RandomProbabilities& probs = probabilities[d->first];
            copy(d->second.parameters().begin(), d->second.parameters().end(),
                inserter(probs, probs.begin()));
          }
        }

        /* Add the likelihood to the counters. */
        for (VertexReferences::iterator child = v->children().begin();
            child != v->children().end(); ++child)
        {
          if ((*child)->value_is_evidence())
          {
            DiscreteRandomVariable& child_value =
            dynamic_cast<DiscreteRandomVariable&>((*child)->random_variable());ConditionalCategoricalDistribution& child_distribution = get<ConditionalCategoricalDistribution>((*child)->distribution());
            DiscreteRandomVariable child_condition = child_distribution.referenced_condition();
            probabilities[child_condition][child_value] += 1.0;
          }
        }

        /* Normalize the counters to probabilities. */
        probabilities.normalize();
      }

    }
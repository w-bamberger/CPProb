#include "discrete_bayesian_network.h"
#include "boolean_random_variable.h"
#include "discrete_joint_random_variable.h"
#include "io_utils.h"
#include "utils.h"
#include <boost/graph/topological_sort.hpp>
#include <tr1/random>

using namespace boost;
using namespace std;
using namespace std::tr1;

namespace vanet
{

  /*
   * All internal methods of the implementation.
   */

  double
  enumerate_all(DiscreteBayesianNetwork& bn,
      vector<DiscreteBayesianNetwork::vertex_descriptor>::iterator begin,
      vector<DiscreteBayesianNetwork::vertex_descriptor>::iterator end);

  void
  learn_multinomial_fill_cpd(DiscreteRandomVariable vertex_var,
      const DiscreteRandomVariable& parents_var,
      const ConditionalCountDistribution& value_count_dis,
      const CountDistribution& parents_count_dis,
      ConditionalCategoricalDistribution& prob_dis);

  void
  learn_multinomial_impl(DiscreteBayesianNetwork& bn,
      const vector<map<string, string> >& data,
      vector<ConditionalCountDistribution>& value_counts,
      vector<CountDistribution>& parent_counts);

  template<class _RandomNumberGenerator>
    void
    sample_from_markov_blanket(DiscreteBayesianNetwork& bn,
        const DiscreteBayesianNetwork::vertex_descriptor& X_v,
        _RandomNumberGenerator& rng);

  CategoricalDistribution
  enumerate(DiscreteBayesianNetwork& bn,
      DiscreteBayesianNetwork::vertex_descriptor X_v)
  {
    vector<DiscreteBayesianNetwork::vertex_descriptor> sorted_vertices(
        num_vertices(bn));
    topological_sort(bn, sorted_vertices.rbegin());
    cout << "Sorted vertices: ";
    for_each(sorted_vertices.begin(), sorted_vertices.end(),
        StreamOut(cout, " ", " "));
    cout << endl;

    CategoricalDistribution X_distribution;
    DiscreteRandomVariable& x = bn[X_v].random_variable;
    bn[X_v].value_is_evidence = true;
    DiscreteRandomVariable::Range X_value_range =
        bn[X_v].random_variable.value_range();
    for (x = X_value_range.begin(); x != X_value_range.end(); ++x)
      {
        X_distribution[x] = enumerate_all(bn, sorted_vertices.begin(),
            sorted_vertices.end());
      }

    X_distribution.normalize();
    return X_distribution;
  }

  double
  enumerate_all(DiscreteBayesianNetwork& bn,
      vector<DiscreteBayesianNetwork::vertex_descriptor>::iterator begin,
      vector<DiscreteBayesianNetwork::vertex_descriptor>::iterator end)
  {
    if (begin == end)
      return 1.0;

    VertexProperties& vp = bn[*begin];
    DiscreteRandomVariable& x = vp.random_variable;

    DiscreteJointRandomVariable parents = parents_variable(bn, *begin);

    if (vp.value_is_evidence)
      {
        double conditional_probability = vp.distribution.at(x, parents);
        return conditional_probability * enumerate_all(bn, ++begin, end);
      }
    else
      {
        double conditional_probability = 0.0;
        double probability_sum = 0.0;
        DiscreteRandomVariable::Range X_value_range =
            vp.random_variable.value_range();

        for (x = X_value_range.begin(); x != X_value_range.end(); ++x)
          {
            conditional_probability = vp.distribution.at(x, parents);
            probability_sum += conditional_probability * enumerate_all(bn,
                begin + 1, end);
          }
        return probability_sum;
      }
  }

  CategoricalDistribution
  gibbs_sampling(DiscreteBayesianNetwork& bn,
      const DiscreteBayesianNetwork::vertex_descriptor& X_v,
      unsigned int iterations)
  {
    RandomNumberEngine rng;
    DiscreteRandomVariable& x = bn[X_v].random_variable;
    CategoricalDistribution X_distribution;

    // @todo Initialise non-evidence variables randomly


    pair<DiscreteBayesianNetwork::vertex_iterator,
        DiscreteBayesianNetwork::vertex_iterator> vertex_range = vertices(bn);
    for (unsigned int iteration = 0; iteration < iterations; ++iteration)
      {
        for (DiscreteBayesianNetwork::vertex_iterator vertex_it =
            vertex_range.first; vertex_it != vertex_range.second; ++vertex_it)
          {
            VertexProperties& vp = bn[*vertex_it];
            if (!vp.value_is_evidence)
              {
                sample_from_markov_blanket(bn, *vertex_it, rng);
                X_distribution[x] += 1.0;
              }
          }
      }

    X_distribution.normalize();
    return X_distribution;
  }

  void
  learn_multinomial_fill_cpd(DiscreteRandomVariable vertex_var,
      const DiscreteRandomVariable& parents_var,
      const ConditionalCountDistribution& value_count_dis,
      const CountDistribution& parents_count_dis,
      ConditionalCategoricalDistribution& prob_dis)
  {
    DiscreteRandomVariable::Range vertex_range = vertex_var.value_range();
    for (vertex_var = vertex_range.begin(); vertex_var != vertex_range.end(); ++vertex_var)
      {
        ConditionalCountDistribution::const_iterator value_count_it =
            value_count_dis.find(make_pair(vertex_var, parents_var));
        if (value_count_it == value_count_dis.end())
          {
            prob_dis.set(vertex_var, parents_var, 0);
          }
        else
          {
            float value_count = static_cast<float> (value_count_it->second);
            float parents_count = static_cast<float> (parents_count_dis.find(
                value_count_it->first.second)->second);
            prob_dis.set(vertex_var, parents_var,
                value_count / parents_count);
          }
      }
  }

  void
  learn_multinomial_impl(DiscreteBayesianNetwork& bn,
      const vector<map<string, string> >& data,
      vector<ConditionalCountDistribution>& value_counts,
      vector<CountDistribution>& parents_counts)
  {
    typedef map<string, string> Variables;
    typedef vector<Variables>::const_iterator DataIterator;

    pair<DiscreteBayesianNetwork::vertex_iterator,
        DiscreteBayesianNetwork::vertex_iterator> vertex_range = vertices(bn);

    /*
     * Walk through the data vector and count the conditional occurrences.
     */
    for (DataIterator data_it = data.begin(); data_it != data.end(); ++data_it)
      {
        const Variables& variables = *data_it;

        /*
         * For every vertex, register the occurrence of the vertex's value
         * together with the parents' values.
         */
        for (DiscreteBayesianNetwork::vertex_iterator vertex_it =
            vertex_range.first; vertex_it != vertex_range.second; ++vertex_it)
          {
            // Determine the value of this vertex.
            const string& var_name = bn[*vertex_it].random_variable.name();
            Variables::const_iterator var_it = variables.find(var_name);
            if (var_it == variables.end())
              throw runtime_error(
                  "learn_ml: Could not find a value for the variable "
                      + var_name + " in the data.");
            BooleanRandomVariable var(var_name, var_it->second);

            // Construct a joint random variable with the values of all parents.
            DiscreteJointRandomVariable parents;
            pair<DiscreteBayesianNetwork::in_edge_iterator,
                DiscreteBayesianNetwork::in_edge_iterator> in_edge_range =
                in_edges(*vertex_it, bn);
            for (DiscreteBayesianNetwork::in_edge_iterator inEIt =
                in_edge_range.first; inEIt != in_edge_range.second; ++inEIt)
              {
                DiscreteBayesianNetwork::vertex_descriptor parent_v = source(
                    *inEIt, bn);
                const string& parent_name = bn[parent_v].random_variable.name();
                Variables::const_iterator parent_it = variables.find(
                    parent_name);
                if (parent_it == variables.end())
                  throw runtime_error(
                      "learn_ml: Could not find a value for the variable "
                          + parent_name + " in the data.");
                BooleanRandomVariable parent(parent_name, parent_it->second);
                parents.insert(parent);
              }

            // Increase the counters for this vertex.
            value_counts.at(*vertex_it)[make_pair(var, parents)] += 1;
            parents_counts.at(*vertex_it)[parents] += 1;
          }
      }

    /*
     * Fill the conditional probability distribution of every vertex.
     */
    for (DiscreteBayesianNetwork::vertex_iterator v_it = vertex_range.first; v_it
        != vertex_range.second; ++v_it)
      {
        CountDistribution& parents_count_dis = parents_counts.at(*v_it);
        ConditionalCountDistribution& value_count_dis = value_counts.at(*v_it);
        ConditionalCategoricalDistribution& prob_dis = bn[*v_it].distribution;
        prob_dis.clear();
        DiscreteRandomVariable vertex_var = bn[*v_it].random_variable;

        DiscreteRandomVariable::Range parents_range = parents_variable(bn,
            *v_it).value_range();
        if (parents_range.empty())
          {
            learn_multinomial_fill_cpd(vertex_var, parents_range.begin(),
                value_count_dis, parents_count_dis, prob_dis);
          }
        else
          {
            for (DiscreteRandomVariable parents_var = parents_range.begin(); parents_var
                != parents_range.end(); ++parents_var)
              {
                learn_multinomial_fill_cpd(vertex_var, parents_var,
                    value_count_dis, parents_count_dis, prob_dis);
              }
          }
      }
  }

  void
  learn_multinomial_map(DiscreteBayesianNetwork& bn,
      const vector<map<string, string> >& data,
      const vector<ConditionalCountDistribution>& prior)
  {
    if (prior.size() != num_vertices(bn))
      {
        ostringstream oss;
        oss << "Invalid prior vector in learn_multinomial_map. Its size is "
            << prior.size() << " but should be " << num_vertices(bn) << ".";
        throw runtime_error(oss.str());
      }

    vector<ConditionalCountDistribution> value_counts(prior);
    vector<CountDistribution> parents_counts(num_vertices(bn));
    vector<ConditionalCountDistribution>::const_iterator prior_it =
        prior.begin();
    vector<CountDistribution>::iterator parents_it = parents_counts.begin();
    for (; prior_it != prior.end(); ++prior_it, ++parents_it)
      {
        for (ConditionalCountDistribution::const_iterator counts_it =
            prior_it->begin(); counts_it != prior_it->end(); ++counts_it)
          {
            (*parents_it)[counts_it->first.second] += counts_it->second;
          }
      }

    learn_multinomial_impl(bn, data, value_counts, parents_counts);
  }

  void
  learn_multinomial_ml(DiscreteBayesianNetwork& bn,
      const vector<map<string, string> >& data)
  {
    vector<ConditionalCountDistribution> value_counts(num_vertices(bn));
    vector<CountDistribution> parents_counts(num_vertices(bn));
    learn_multinomial_impl(bn, data, value_counts, parents_counts);
  }

  ostream&
  operator<<(ostream& os, const DiscreteBayesianNetwork& bn)
  {
    os << "Vertices of the Bayesian network:\n";
    pair<DiscreteBayesianNetwork::vertex_iterator,
        DiscreteBayesianNetwork::vertex_iterator> vertices_range =
        boost::vertices(bn);
    for (DiscreteBayesianNetwork::vertex_iterator vIt = vertices_range.first; vIt
        != vertices_range.second; ++vIt)
      {
        const VertexProperties& vp = bn[*vIt];
        os << "  Vertex " << *vIt;
        if (vp.value_is_evidence)
          os << " with value " << vp.random_variable << "\n";

        else
          os << " of variable " << vp.random_variable.name()
              << " with no evidence\n";

        for_each(vp.distribution.begin(), vp.distribution.end(),
            StreamOut(os, "    "));
      }

    os << "Edges of the Bayesian network:\n";
    pair<DiscreteBayesianNetwork::edge_iterator,
        DiscreteBayesianNetwork::edge_iterator> edges_range = boost::edges(bn);
    for (DiscreteBayesianNetwork::edge_iterator eIt = edges_range.first; eIt
        != edges_range.second; ++eIt)
      {
        os << "  Edge " << *eIt << "\n";
      }
    os << endl;
    return os;
  }

  DiscreteJointRandomVariable
  parents_variable(const DiscreteBayesianNetwork& bn,
      DiscreteBayesianNetwork::vertex_descriptor v)
  {
    DiscreteJointRandomVariable parents;
    DiscreteBayesianNetwork::in_edge_iterator edge_it, edge_end_it;
    tie(edge_it, edge_end_it) = in_edges(v, bn);
    for (; edge_it != edge_end_it; ++edge_it)
      {
        DiscreteBayesianNetwork::vertex_descriptor parent =
            source(*edge_it, bn);
        parents.insert(bn[parent].random_variable);
      }
    return parents;
  }

  template<class _RandomNumberGenerator>
    void
    sample_from_markov_blanket(DiscreteBayesianNetwork& bn,
        const DiscreteBayesianNetwork::vertex_descriptor& X_v,
        _RandomNumberGenerator& rng)
    {
      VertexProperties& vp = bn[X_v];
      DiscreteRandomVariable& x = vp.random_variable;
      CategoricalDistribution sampling_distribution;

      DiscreteRandomVariable::Range x_range = vp.random_variable.value_range();
      for (x = x_range.begin(); x != x_range.end(); ++x)
        {
          float p = vp.distribution.at(x, parents_variable(bn, X_v));

          DiscreteBayesianNetwork::out_edge_iterator edge_it, edge_end_it;
          tie(edge_it, edge_end_it) = out_edges(X_v, bn);
          for (; edge_it != edge_end_it; ++edge_it)
            {
              DiscreteBayesianNetwork::vertex_descriptor child_v = target(
                  *edge_it, bn);
              VertexProperties& child_vp = bn[child_v];
              p *= child_vp.distribution.at(child_vp.random_variable,
                  parents_variable(bn, child_v));
            }

          sampling_distribution[x] = p;
        }

      sampling_distribution.normalize();
      std::tr1::variate_generator<_RandomNumberGenerator&,
          CategoricalDistribution> sampling_variate(rng, sampling_distribution);
      x = sampling_variate();
    }

}

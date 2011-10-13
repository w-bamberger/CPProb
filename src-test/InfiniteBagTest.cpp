/*
 * InfiniteBagTest.cpp
 *
 *  Created on: 07.10.2011
 *      Author: wbam
 */

#include "CsvMapReader.hpp"
#include "../src-lib/BayesianNetwork.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include <boost/program_options.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>

using namespace cpprob;
using namespace std;

extern boost::program_options::variables_map options_map;

class InfiniteBagFixture
{

public:

  InfiniteBagFixture()
      : alpha(5.0)
  {
    BOOST_REQUIRE_MESSAGE(options_map.count("data-file") == 1,
        "Please provide test data with the option --data-file.");
    data_file_ = options_map["data-file"].as<string>();

    correct_flavor_distribution_ = flavor_counts();
    for (auto entry = correct_flavor_distribution_.begin();
        entry != correct_flavor_distribution_.end(); ++entry)
    {
      entry->second += alpha;
    }
    correct_flavor_distribution_.normalize();
  }

  /**
   * Provides the number of lines in the file @c data_file_ for the
   * following conditions:
   * - C(F=0 | H=1, W=1) (grep -e ",false,true,true" latent-bag.csv | wc) and
   * - C(F=1 | H=1, W=1) (grep -e ",true,true,true" latent-bag.csv | wc).
   * The counts were determined with the command in parenthesis (an example
   * for the file latent-bag.csv).
   *
   * The counts (e.g., 52/20) together with the prior parameters (e.g., 5/5)
   * lead to the total count (57/25). Normalizing the total count leads to
   * the conditional probabilities of the given file.
   */
  initializer_list<CategoricalDistribution::value_type>
  flavor_counts()
  {
    const RandomBoolean flavor_false("Flavor", false);
    const RandomBoolean flavor_true("Flavor", true);

    if (data_file_ == "latent-bag-short.csv")
      return
      {
        { flavor_false, 6.0},
        { flavor_true, 2.0} //
      };

    else if (data_file_ == "latent-bag.csv")
      /*
       * The counts in latent-bag.csv are
       *   C(F=0 | H=1, W=1): 52 (grep -e ",false,true,true" latent-bag.csv | wc)
       *   C(F=1 | H=1, W=1): 20 (grep -e ",true,true,true" latent-bag.csv | wc)
       */
      return
      {
        { flavor_false, 52.0},
        { flavor_true, 20.0} //
      };

    else if (data_file_ == "latent-bag-long.csv")
      return
      {
        { flavor_false, 2534.0},
        { flavor_true, 1280.0} //
      };

    else
      return
      {};
  }

  BayesianNetwork
  gen_infinite_bag_net(
      size_t lines_of_evidence = std::numeric_limits<std::size_t>::max())
  {
    const DiscreteRandomReferences no_condition;
    BayesianNetwork bn;

    // Set up the parameter vertices
    map<string, ConditionalDirichletNode*> params_table;
    RandomBoolean bag("Bag", true);
    RandomConditionalProbabilities flavor_params(RandomBoolean("Flavor", true),
        bag);
    params_table.insert(
        make_pair("Flavor",
            &bn.add_conditional_dirichlet(flavor_params, alpha)));
    RandomConditionalProbabilities wrapper_params(
        RandomBoolean("Wrapper", true), bag);
    params_table.insert(
        make_pair("Wrapper",
            &bn.add_conditional_dirichlet(wrapper_params, alpha)));
    RandomConditionalProbabilities hole_params(RandomBoolean("Hole", true),
        bag);
    params_table.insert(
        make_pair("Hole", &bn.add_conditional_dirichlet(hole_params, alpha)));
    ConstantNode<DirichletProcessParameters>& bag_params_node =
        bn.add_dirichlet_process_parameters(bag.name(), 2.0,
            boost::adaptors::values(params_table));

    // Read the data from the file
    CsvMapReader reader(data_file_);
    CsvMapReader::Records full_data = reader.read_rows();

    // Fill the data in the net
    size_t line = 0;
    CsvMapReader::Records::iterator r = full_data.begin();
    for (; r != full_data.end() && line != lines_of_evidence; ++r, ++line)
    {
      RandomBoolean bag_evidence("Bag", r->operator[]("Bag"));
      DirichletProcessNode& bag_evidence_node = bn.add_dirichlet_process(
          bag_params_node);

      for (CsvMapReader::NamedAttributes::iterator a = r->begin();
          a != r->end(); ++a)
      {
        if (a->first != "Bag")
        {
          RandomBoolean var(a->first, a->second);
          ConditionalCategoricalNode& node = bn.add_conditional_categorical(var,
            { &bag_evidence_node }, *params_table[a->first]);
          node.is_evidence(true);
        }
      }
    }

    return bn;
  }

  template<class K, class V>
    ostream&
    put_out_map(ostream& os, const cont::map<K, V>& m)
    {
      for (auto it = m.begin(); it != m.end(); ++it)
        os << it->first << ", " << it->second << "\n";
      return os;
    }

protected:

  const float alpha;
  CategoricalDistribution correct_flavor_distribution_;
  string data_file_;

};

BOOST_FIXTURE_TEST_CASE(InfiniteBagTest, InfiniteBagFixture)
{
  /*
   * Generate the network from the data file.
   */
  cout << "Generate the bag network with infinite mixtures\n";
  double duration;
  boost::timer t;
  BayesianNetwork bn = gen_infinite_bag_net();
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << endl;

  if (options_map["with-debug-output"].as<bool>())
    cout << bn << endl;

  /*
   * Extend the network for the reasoning task.
   *
   * An additional hole and wrapper node serves as evidence for an unknown
   * flavor node.
   */
  ConstantNode<DirichletProcessParameters>& bag_params = bn.find<
      ConstantNode<DirichletProcessParameters> >("Bag");
  DirichletProcessNode& bag_v = bn.add_dirichlet_process(bag_params);

  ConditionalDirichletNode& hole_params_v = bn.find<ConditionalDirichletNode>(
      "ProbabilitiesHoleBag");
  RandomBoolean hole("Hole", true);
  ConditionalCategoricalNode& hole_v = bn.add_conditional_categorical(hole,
    { &bag_v }, hole_params_v);
  hole_v.is_evidence(true);

  ConditionalDirichletNode& wrapper_params_v =
      bn.find<ConditionalDirichletNode>("ProbabilitiesWrapperBag");
  RandomBoolean wrapper("Wrapper", true);
  ConditionalCategoricalNode& wrapper_v = bn.add_conditional_categorical(
      wrapper,
        { &bag_v }, wrapper_params_v);
  wrapper_v.is_evidence(true);

  ConditionalDirichletNode& flavor_params_v = bn.find<ConditionalDirichletNode>(
      "ProbabilitiesFlavorBag");
  RandomBoolean flavor("Flavor", true);
  ConditionalCategoricalNode& flavor_v = bn.add_conditional_categorical(flavor,
    { &bag_v }, flavor_params_v);

  /*
   * Predict the unknown flavor node by sampling.
   */
  unsigned int burn_in_iterations = options_map["burn-in-iterations"].as<
      unsigned int>();
  unsigned int collect_iterations = options_map["collect-iterations"].as<
      unsigned int>();
  random_number_engine.seed(); // Reset in a well-defined state.
  cout << "Sample with " << burn_in_iterations << " burn-in iterations and "
      << collect_iterations << " collect iterations.\n";
  t.restart();
  CategoricalDistribution prediction = bn.sample(flavor_v, burn_in_iterations,
      collect_iterations);
  duration = t.elapsed();
  if (!options_map["test-mode"].as<bool>())
    cout << "Duration: " << duration << "\n";
  cout << "Predictive distribution with sampling:\n";
  cout << prediction << endl;

  if (correct_flavor_distribution_.size() != 0)
  {
    cout << "Correct values (" << data_file_ << "):\n";
    cout << correct_flavor_distribution_ << endl;
    BOOST_CHECK_CLOSE(prediction.begin()->second,
        correct_flavor_distribution_.begin()->second, 5);
  }

  if (options_map["with-debug-output"].as<bool>())
    put_out_map(cout, bag_params.value().component_counters());
}


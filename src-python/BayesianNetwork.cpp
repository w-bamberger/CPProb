#include <boost/python/class.hpp>
#include <boost/python/list.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/tuple.hpp>
#include <cpprob/BayesianNetwork.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    boost::python::tuple
    bayesian_network_sample_automatic(BayesianNetwork& bn,
        const DiscreteNode& X, float max_deviation)
    {
      boost::python::list result;
      unsigned int iterations;
      result.append(bn.sample(X, max_deviation, &iterations));
      result.append(iterations);
      return boost::python::tuple(result);
    }

    void
    export_bayesian_network()
    {
      CategoricalNode&
      (BayesianNetwork::*add_categorical)(
          const DiscreteRandomVariable&) = &BayesianNetwork::add_categorical;

      CategoricalNode&
      (BayesianNetwork::*add_categorical_with_dirichlet)(
          const DiscreteRandomVariable&, DirichletNode&)
          = &BayesianNetwork::add_categorical;

      ConditionalCategoricalNode&
      (BayesianNetwork::*add_conditional_categorical_with_dirichlet)(
          const DiscreteRandomVariable&, const cont::RefVector<DiscreteNode>&,
          ConditionalDirichletNode&) =
          &BayesianNetwork::add_conditional_categorical;

      ConditionalCategoricalNode&
      (BayesianNetwork::*add_conditional_categorical_with_constant_node)(
          const DiscreteRandomVariable&, const cont::RefVector<DiscreteNode>&,
          ConstantRandomConditionalProbabilitiesNode&) =
          &BayesianNetwork::add_conditional_categorical;

      ConstantDiscreteRandomVariableNode&
      (BayesianNetwork::*add_constant_discrete_random_variable)(
          const DiscreteRandomVariable&) = &BayesianNetwork::add_constant;

      ConstantRandomConditionalProbabilitiesNode&
      (BayesianNetwork::*add_constant_random_conditional_probabilities)(
          const RandomConditionalProbabilities&) =
          &BayesianNetwork::add_constant;

      std::size_t
      (BayesianNetwork::*erase_categorical)(
          const CategoricalNode&) = &BayesianNetwork::erase<CategoricalNode>;

      std::size_t
      (BayesianNetwork::*erase_conditional_categorical)(
          const ConditionalCategoricalNode&) =
          &BayesianNetwork::erase<ConditionalCategoricalNode>;

      std::size_t
      (BayesianNetwork::*erase_conditional_dirichlet)(
          const ConditionalDirichletNode&) =
          &BayesianNetwork::erase<ConditionalDirichletNode>;

      std::size_t
      (BayesianNetwork::*erase_dirichlet)(
          const DirichletNode&) = &BayesianNetwork::erase<DirichletNode>;

      CategoricalDistribution
      (BayesianNetwork::*sample_manual)(const DiscreteNode&, unsigned int,
          unsigned int) = &BayesianNetwork::sample;

      class_ < BayesianNetwork
          > ("BayesianNetwork") //
          .def(init<BayesianNetwork>()) //
          .def("__len__", &BayesianNetwork::size) //
          .def(self_ns::str(self_ns::self)) //
          .def("add_categorical", add_categorical,
              return_internal_reference<>()) //
          .def("add_categorical", add_categorical_with_dirichlet,
              return_internal_reference<>()) //
          .def("add_conditional_categorical",
              add_conditional_categorical_with_dirichlet,
              return_internal_reference<>()) //
          .def("add_conditional_categorical",
              add_conditional_categorical_with_constant_node,
              return_internal_reference<>()) //
          .def("add_conditional_dirichlet",
              &BayesianNetwork::add_conditional_dirichlet,
              return_internal_reference<>()) //
          .def("add_constant", add_constant_discrete_random_variable,
              return_internal_reference<>()) //
          .def("add_constant", add_constant_random_conditional_probabilities,
              return_internal_reference<>()) //
          .def("add_dirichlet", &BayesianNetwork::add_dirichlet,
              return_internal_reference<>()) //
          .def("erase", erase_categorical) //
          .def("erase", erase_conditional_categorical) //
          .def("erase", erase_conditional_dirichlet) //
          .def("erase", erase_dirichlet) //
          .def("sample", sample_manual) //
          .def("sample", &bayesian_network_sample_automatic);
    }

  }
}

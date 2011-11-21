#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <cpprob/BayesianNetwork.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

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
          const DiscreteRandomVariable&,
          const cont::RefVector<DiscreteNode>&,
          ConditionalDirichletNode&) = &BayesianNetwork::add_conditional_categorical;

      ConstantDiscreteRandomVariableNode&
      (BayesianNetwork::*add_constant_discrete_random_variable)(
          const DiscreteRandomVariable&) = &BayesianNetwork::add_constant;

      std::size_t
      (BayesianNetwork::*erase_categorical)(
          const CategoricalNode&) = &BayesianNetwork::erase<CategoricalNode>;

      std::size_t
      (BayesianNetwork::*erase_conditional_categorical)(
          const ConditionalCategoricalNode&) = &BayesianNetwork::erase<ConditionalCategoricalNode>;

      std::size_t
      (BayesianNetwork::*erase_conditional_dirichlet)(
          const ConditionalDirichletNode&) = &BayesianNetwork::erase<ConditionalDirichletNode>;

      std::size_t
      (BayesianNetwork::*erase_dirichlet)(
          const DirichletNode&) = &BayesianNetwork::erase<DirichletNode>;

      class_ < BayesianNetwork
          > ("BayesianNetwork") //
          .def(init<BayesianNetwork>())
          .def("sample", &BayesianNetwork::sample) //
          .def("__len__", &BayesianNetwork::size) //
          .def(self_ns::str(self_ns::self))//
          .def("add_categorical", add_categorical,
              return_internal_reference<>()) //
          .def("add_categorical", add_categorical_with_dirichlet,
              return_internal_reference<>()) //
          .def("add_conditional_categorical",
              add_conditional_categorical_with_dirichlet,
              return_internal_reference<>()) //
          .def("add_conditional_dirichlet",
              &BayesianNetwork::add_conditional_dirichlet,
              return_internal_reference<>()) //
          .def("add_constant", add_constant_discrete_random_variable,
              return_internal_reference<>()) //
          .def("add_dirichlet", &BayesianNetwork::add_dirichlet,
              return_internal_reference<>()) //
          .def("erase", erase_categorical) //
          .def("erase", erase_conditional_categorical) //
          .def("erase", erase_conditional_dirichlet) //
          .def("erase", erase_dirichlet);
    }

  }
}

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
      (BayesianNetwork::*add_categorical_with_dirichlet)(
          const DiscreteRandomVariable&, DirichletNode&)
          = &BayesianNetwork::add_categorical;

      ConditionalCategoricalNode&
      (BayesianNetwork::*add_conditional_categorical_with_dirichlet)(
          const DiscreteRandomVariable&,
          const std::list<DiscreteNode*>&,
          ConditionalDirichletNode&) = &BayesianNetwork::add_conditional_categorical;

      ConstantNode<DiscreteRandomVariable>&
      (BayesianNetwork::*add_constant_discrete_random_variable)(
          const DiscreteRandomVariable&) = &BayesianNetwork::add_constant;

      class_ < BayesianNetwork
          > ("BayesianNetwork") //
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
          .def("sample", &BayesianNetwork::sample) //
          .def(self_ns::str(self_ns::self));
    }

  }
}

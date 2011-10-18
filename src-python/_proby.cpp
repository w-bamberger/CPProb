#include "ListListConverter.hpp"
#include <boost/python.hpp>
#include <cpprob/BayesianNetwork.hpp>
#include <cpprob/RandomBoolean.hpp>
#include <cpprob/RandomInteger.hpp>
#include <cpprob/RandomProbabilities.hpp>

char const*
greet()
{
  return "hello, world";
}

BOOST_PYTHON_MODULE(_proby)
{
  using namespace boost::python;
  using namespace cpprob;

  def("greet", greet);

  /* BayesianNetwork */
  CategoricalNode&
  (BayesianNetwork::*bayesian_network_add_categorical_with_dirichlet)(
      const DiscreteRandomVariable&, DirichletNode&)
      = &BayesianNetwork::add_categorical;
  ConditionalCategoricalNode&
  (BayesianNetwork::*bayesian_network_add_conditional_categorical_with_dirichlet)(
      const DiscreteRandomVariable&,
      const std::list<DiscreteNode*>&,
      ConditionalDirichletNode&) = &BayesianNetwork::add_conditional_categorical;
  class_<BayesianNetwork>("BayesianNetwork")
  .def("add_categorical", bayesian_network_add_categorical_with_dirichlet, return_internal_reference<>())
  .def("add_conditional_categorical", bayesian_network_add_conditional_categorical_with_dirichlet, return_internal_reference<>())
  .def("add_conditional_dirichlet", &BayesianNetwork::add_conditional_dirichlet, return_internal_reference<>())
  .def("add_dirichlet", &BayesianNetwork::add_dirichlet, return_internal_reference<>())
  .def("sample", &BayesianNetwork::sample)
  .def(self_ns::str(self_ns::self))
  ;

  /* DiscreteNode. */
  class_<DiscreteNode, boost::noncopyable>("DiscreteNode", no_init);
  ListListConverter<DiscreteNode*>();

  /* CategoricalDistribution */
  class_<CategoricalDistribution>("CategoricalDistribution")
  .def(self_ns::str(self_ns::self))
  ;

  /* CategoricalNode */
  class_<CategoricalNode, bases<DiscreteNode> >("CategoricalNode", no_init);

  /* ConditionalCategoricalNode */
  bool
  (ConditionalCategoricalNode::*conditional_categorical_node_get_is_evidence)() const =
  &ConditionalCategoricalNode::is_evidence;
  void
  (ConditionalCategoricalNode::*conditional_categorical_node_set_is_evidence)(
      bool) =
      &ConditionalCategoricalNode::is_evidence;
  class_<ConditionalCategoricalNode, bases<DiscreteNode> >("ConditionalCategoricalNode", no_init)
  .add_property("is_evidence", conditional_categorical_node_get_is_evidence,
      conditional_categorical_node_set_is_evidence);

  /* ConditionalDirichletNode */
  class_<ConditionalDirichletNode>("ConditionalDirichletNode", no_init);

  /* DirichletNode */
  class_<DirichletNode>("DirichletNode", no_init);

  /* DiscreteRandomVariable */
  class_<DiscreteRandomVariable>("DiscreteRandomVariable")
  .add_property("name", make_function(&DiscreteRandomVariable::name, return_value_policy<copy_const_reference>()))
  ;

  /* RandomConditionalProbabilities */
  class_<RandomConditionalProbabilities>("RandomConditionalProbabilities", init<DiscreteRandomVariable, DiscreteRandomVariable>());

  /* RandomBoolean */
  class_<RandomBoolean, bases<DiscreteRandomVariable> >("RandomBoolean", init<std::string, bool>())
  .def(init<std::string,std::string>())
  ;

  /* RandomInteger */
  std::size_t
  (RandomInteger::*random_integer_get_observation)(
      void) const = &RandomInteger::observation;
  void
  (RandomInteger::*random_integer_set_observation)(
      std::size_t) = &RandomInteger::observation;

  class_<RandomInteger, bases<DiscreteRandomVariable> >("RandomInteger", init<std::string, std::size_t, std::size_t>())
  .add_property("observation", random_integer_get_observation, random_integer_set_observation)
  ;

  /* RandomProbabilities */
  class_<RandomProbabilities>("RandomProbabilities", init<DiscreteRandomVariable>())
  ;
}

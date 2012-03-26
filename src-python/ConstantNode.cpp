#include <boost/python/class.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <cpprob/ConstantNode.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    DiscreteRandomVariable&
    (ConstantDiscreteRandomVariableNode::*discrete_random_variable_value)() =
    &ConstantDiscreteRandomVariableNode::value;

    RandomConditionalProbabilities&
    (ConstantRandomConditionalProbabilitiesNode::*random_conditional_probabilities_value)() =
    &ConstantRandomConditionalProbabilitiesNode::value;

    void
    export_constant_node()
    {
      class_ < ConstantDiscreteRandomVariableNode, bases<DiscreteNode>
          > ("ConstantDiscreteRandomVariableNode", no_init) //
          .def("value", discrete_random_variable_value,
              return_internal_reference<>());

      class_ < ConstantRandomConditionalProbabilitiesNode
          > ("ConstantRandomConditionalProbabilitiesNode", no_init) //
          .def("value", random_conditional_probabilities_value,
              return_internal_reference<>());
    }

  }
}

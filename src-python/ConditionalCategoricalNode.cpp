#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <cpprob/ConditionalCategoricalNode.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_conditional_categorical_node()
    {
      bool
      (ConditionalCategoricalNode::*get_is_evidence)() const =
      &ConditionalCategoricalNode::is_evidence;

      void
      (ConditionalCategoricalNode::*set_is_evidence)(bool) =
      &ConditionalCategoricalNode::is_evidence;

      DiscreteRandomVariable&
      (ConditionalCategoricalNode::*value)() =
      &ConditionalCategoricalNode::value;

      class_ < ConditionalCategoricalNode, bases<DiscreteNode>
          > ("ConditionalCategoricalNode", no_init) //
          .add_property("is_evidence", get_is_evidence, set_is_evidence) //
          .def(self_ns::str(self_ns::self)) //
          .def("value", value, return_internal_reference<>());
    }

  }
}

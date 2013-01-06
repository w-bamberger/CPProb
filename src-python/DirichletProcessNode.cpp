#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <cpprob/DirichletProcessNode.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_dirichlet_process_node()
    {
      bool
      (ConditionalCategoricalNode::*get_is_evidence)() const =
      &ConditionalCategoricalNode::is_evidence;

      DiscreteRandomVariable&
      (ConditionalCategoricalNode::*value)() =
      &ConditionalCategoricalNode::value;

      class_ < DirichletProcessNode, bases<DiscreteNode>
          > ("DirichletProcessNode", no_init) //
          .add_property("is_evidence", get_is_evidence) //
          .def(self_ns::str(self_ns::self)) //
          .def("value", value, return_internal_reference<>());
    }

  }
}

#include <boost/python/class.hpp>
#include <cpprob/CategoricalNode.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_categorical_node()
    {
      bool
      (CategoricalNode::*get_is_evidence)() const = &CategoricalNode::is_evidence;

      void
      (CategoricalNode::*set_is_evidence)(bool) = &CategoricalNode::is_evidence;

      DiscreteRandomVariable&
      (CategoricalNode::*value)() = &CategoricalNode::value;

      class_ < CategoricalNode, bases<DiscreteNode>
          > ("CategoricalNode", no_init) //
          .add_property("is_evidence", get_is_evidence, set_is_evidence) //
          .def("value", value, return_internal_reference<>());
    }

  }
}

#include "ListListConverter.hpp"
#include "ListRefVectorConverter.hpp"
#include <boost/python/class.hpp>
#include <cpprob/ConditionalDirichletNode.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_conditional_dirichlet_node()
    {
      RandomConditionalProbabilities&
      (ConditionalDirichletNode::*value)() = &ConditionalDirichletNode::value;

      class_ < ConditionalDirichletNode > ("ConditionalDirichletNode", no_init) //
      .def("value", value, return_internal_reference<>());

      ListListConverter<ConditionalDirichletNode>();
      ListRefVectorConverter<ConditionalDirichletNode>();
    }

  }
}

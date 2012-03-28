#include <boost/python/class.hpp>
#include <cpprob/DirichletNode.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_dirichlet_node()
    {
      RandomProbabilities&
      (DirichletNode::*value)() = &DirichletNode::value;

      class_ < DirichletNode > ("DirichletNode", no_init) //
      .def("value", value, return_internal_reference<>());
    }

  }
}

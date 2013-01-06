#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <cpprob/DirichletProcessParameters.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_dirichlet_process_parameters()
    {
      class_ <DirichletProcessParameters
          > ("DirichletProcessParameters", no_init) //
          .def(self_ns::str(self_ns::self));
    }

  }
}

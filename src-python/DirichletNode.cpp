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
      class_ < DirichletNode > ("DirichletNode", no_init);
    }

  }
}

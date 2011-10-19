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
      class_ < ConditionalDirichletNode > ("ConditionalDirichletNode", no_init);
    }

  }
}

#include <boost/python/class.hpp>
#include <cpprob/ConstantNode.hpp>
#include <cpprob/DiscreteRandomVariable.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_constant_node()
    {
      class_ < ConstantNode<DiscreteRandomVariable>, bases<DiscreteNode>
          > ("ConstantDiscreteRandomVariableNode", no_init);
    }

  }
}

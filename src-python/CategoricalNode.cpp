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
      class_ < CategoricalNode, bases<DiscreteNode>
          > ("CategoricalNode", no_init);
    }

  }
}

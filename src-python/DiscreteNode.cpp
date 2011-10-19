#include "ListListConverter.hpp"
#include <boost/python/class.hpp>
#include <cpprob/DiscreteNode.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_discrete_node()
    {
      class_ < DiscreteNode, boost::noncopyable > ("DiscreteNode", no_init);
      ListListConverter<DiscreteNode*>();
    }

  }
}

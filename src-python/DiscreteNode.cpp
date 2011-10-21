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
      bool
      (DiscreteNode::*get_is_evidence)() const = &DiscreteNode::is_evidence;

      void
      (DiscreteNode::*set_is_evidence)(bool) = &DiscreteNode::is_evidence;

      class_ < DiscreteNode, boost::noncopyable > ("DiscreteNode", no_init) //
      .add_property("is_evidence", get_is_evidence, set_is_evidence);
      ListListConverter<DiscreteNode*>();
    }

  }
}

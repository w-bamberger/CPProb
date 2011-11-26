#include "ListListConverter.hpp"
#include "ListRefVectorConverter.hpp"
#include <boost/python/class.hpp>
#include <cpprob/DiscreteNode.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

	DiscreteRandomVariable
	discrete_node_get_value(DiscreteNode& node)
	{
	  return node.value();
	}

	void
	discrete_node_set_value(DiscreteNode& node, 
							const DiscreteRandomVariable& value)
	{
	  node.value() = value;
	}

    void
    export_discrete_node()
    {
      class_ < DiscreteNode, boost::noncopyable > ("DiscreteNode", no_init) //
		.add_property("value", &discrete_node_get_value, 
					  &discrete_node_set_value);

      ListListConverter<DiscreteNode*>();
      ListRefVectorConverter<DiscreteNode>();
    }

  }
}

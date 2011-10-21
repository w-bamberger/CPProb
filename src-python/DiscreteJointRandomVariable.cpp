#include "SetHelper.hpp"
#include <boost/python/class.hpp>
#include <cpprob/DiscreteJointRandomVariable.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_discrete_joint_random_variable()
    {
      class_ < DiscreteJointRandomVariable, bases<DiscreteRandomVariable>
          > ("DiscreteJointRandomVariable") //
          .def("__len__", &DiscreteJointRandomVariable::size) //
          .def("add", &SetHelper < DiscreteJointRandomVariable > ::add);
    }

  }
}

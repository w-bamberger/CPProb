#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/operators.hpp>
#include <cpprob/DiscreteRandomVariable.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_discrete_random_variable()
    {
      class_ < DiscreteRandomVariable
          > ("DiscreteRandomVariable") //
          .add_property(
              "name",
              make_function(&DiscreteRandomVariable::name,
                  return_value_policy<copy_const_reference>())) //
          .def(self_ns::str(self_ns::self));
    }

  }
}

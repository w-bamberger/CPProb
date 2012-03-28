#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/list.hpp>
#include <boost/python/operators.hpp>
#include <cpprob/DiscreteRandomVariable.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    boost::python::list
    discrete_random_variable_value_range(const DiscreteRandomVariable var)
    {
      boost::python::list l;
      for (auto v = var.value_range().begin(); v != var.value_range().end();
          ++v)
        l.append(v);
      return l;
    }

    void
    export_discrete_random_variable()
    {
      class_ < DiscreteRandomVariable
          > ("DiscreteRandomVariable") //
          .add_property(
              "name",
              make_function(&DiscreteRandomVariable::name,
                  return_value_policy<copy_const_reference>())) //
          .def(self_ns::str(self_ns::self)) //
          .def("value_range", &discrete_random_variable_value_range);
    }

  }
}

#include "MapHelper.hpp"
#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/operators.hpp>
#include <cpprob/RandomProbabilities.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_random_probabilities()
    {
      class_ < RandomProbabilities
          > ("RandomProbabilities", init<DiscreteRandomVariable>()) //
          .add_property(
              "name",
              make_function(&RandomProbabilities::name,
                  return_value_policy<copy_const_reference>())) //
          .def(self_ns::str(self_ns::self)) //
          .def("__getitem__", &MapHelper < RandomProbabilities > ::get,
              return_value_policy<copy_non_const_reference>()) //
          .def("__len__", &RandomProbabilities::size) //
          .def("__setitem__", &MapHelper < RandomProbabilities > ::set,
              with_custodian_and_ward<1, 2>()) //
          .def("__contains__", &MapHelper < RandomProbabilities > ::contains) //
          .def("clear", &RandomProbabilities::clear) //
          .def("items", &MapHelper < RandomProbabilities > ::items) //
          .def("keys", &MapHelper < RandomProbabilities > ::keys) //
          .def("values", &MapHelper < RandomProbabilities > ::values) //
          .def("normalize", &RandomProbabilities::normalize);
    }

  }
}

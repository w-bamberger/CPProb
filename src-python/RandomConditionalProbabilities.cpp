#include "MapHelper.hpp"
#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <cpprob/RandomConditionalProbabilities.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_random_conditional_probabilities()
    {
      class_ < RandomConditionalProbabilities
          > ("RandomConditionalProbabilities", init<DiscreteRandomVariable,
              DiscreteRandomVariable>()) //
          .add_property(
              "name",
              make_function(&RandomConditionalProbabilities::name,
                  return_value_policy<copy_const_reference>())) //
          .def(self_ns::str(self_ns::self)) //
          .def("__getitem__",
              &MapHelper < RandomConditionalProbabilities > ::get,
              return_internal_reference<>()) //
          .def("__len__", &RandomConditionalProbabilities::size) //
          .def("__setitem__",
              &MapHelper < RandomConditionalProbabilities > ::set,
              with_custodian_and_ward<1, 2>()) // to let container keep value
          .def("__contains__",
              &MapHelper < RandomConditionalProbabilities > ::contains) //
          .def("clear", &RandomConditionalProbabilities::clear) //
          .def("items", &MapHelper < RandomConditionalProbabilities > ::items) //
          .def("keys", &MapHelper < RandomConditionalProbabilities > ::keys) //
          .def("values", &MapHelper < RandomConditionalProbabilities > ::values) //
          .def("normalize", &RandomConditionalProbabilities::normalize);
    }

  }
}

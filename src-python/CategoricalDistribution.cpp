#include "MapHelper.hpp"
#include <boost/python/class.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/with_custodian_and_ward.hpp>
#include <boost/python/def.hpp>
#include <boost/python/operators.hpp>
#include <cpprob/CategoricalDistribution.hpp>

using namespace cpprob;
using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_categorical_distribution()
    {
      class_ < CategoricalDistribution
          > ("CategoricalDistribution") //
          .def(self_ns::str(self_ns::self)) //
          .def("__getitem__", &MapHelper < CategoricalDistribution > ::get,
              return_value_policy<copy_non_const_reference>()) //
          .def("__len__", &CategoricalDistribution::size) //
          .def("__setitem__", &MapHelper < CategoricalDistribution > ::set,
              with_custodian_and_ward<1, 2>()) // to let container keep value
          .def("__contains__",
              &MapHelper < CategoricalDistribution > ::contains) //
          .def("clear", &CategoricalDistribution::clear) //
          .def("items", &MapHelper < CategoricalDistribution > ::items) //
          .def("keys", &MapHelper < CategoricalDistribution > ::keys) //
          .def("values", &MapHelper < CategoricalDistribution > ::values);

      float
      (*mean_ptr)(const CategoricalDistribution&) = &mean;
      def("mean", mean_ptr);

	  float
	  (*standard_deviation_ptr)(const CategoricalDistribution&) = &standard_deviation;
	  def("standard_deviation", standard_deviation_ptr);

      float
      (*variance_ptr)(const CategoricalDistribution&) = &variance;
      def("variance", variance_ptr);

    }

  }
}

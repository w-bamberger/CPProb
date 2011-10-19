#include <boost/python/class.hpp>
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
      class_ < CategoricalDistribution > ("CategoricalDistribution") //
      .def(self_ns::str(self_ns::self));
    }

  }
}

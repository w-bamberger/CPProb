#include <boost/python/class.hpp>
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
          .def(self_ns::str(self_ns::self));
    }

  }
}

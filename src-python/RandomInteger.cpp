#include <boost/python/class.hpp>
#include <cpprob/RandomInteger.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_random_integer()
    {
      std::size_t
      (RandomInteger::*get_observation)(
          void) const = &RandomInteger::observation;

      void
      (RandomInteger::*set_observation)(
          std::size_t) = &RandomInteger::observation;

      class_ < RandomInteger, bases<DiscreteRandomVariable>
          > ("RandomInteger", init<std::string, std::size_t, std::size_t>()) //
          .add_property("observation", get_observation, set_observation);
    }

  }
}

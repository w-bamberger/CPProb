#include <boost/python/class.hpp>
#include <cpprob/RandomInteger.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    random_integer_set_observation(RandomInteger& ri, std::size_t value)
    {
      ri.observation(value);
    }

    void
    export_random_integer()
    {
      std::size_t
      (RandomInteger::*get_observation)(
          void) const = &RandomInteger::observation;

      class_ < RandomInteger, bases<DiscreteRandomVariable>
          > ("RandomInteger", init<std::string, std::size_t, std::size_t>()) //
          .add_property("observation", get_observation,
              &random_integer_set_observation);
    }

  }
}

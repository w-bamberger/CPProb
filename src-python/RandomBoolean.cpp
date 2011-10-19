#include <boost/python/class.hpp>
#include <cpprob/RandomBoolean.hpp>

using namespace boost::python;

namespace cpprob
{
  namespace proby
  {

    void
    export_random_boolean()
    {
      class_ < RandomBoolean, bases<DiscreteRandomVariable>
          > ("RandomBoolean", init<std::string, bool>()).def(
              init<std::string, std::string>());
    }

  }
}

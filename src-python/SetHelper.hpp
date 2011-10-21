#include <boost/python/base_type_traits.hpp>

namespace cpprob
{
  namespace proby
  {

    template<class SetType>
      class SetHelper
      {

      public:

        typedef typename SetType::value_type value_type;

        static void
        add(SetType& s, const value_type& x)
        {
          if (!s.insert(x).second)
            PyErr_SetString(PyExc_RuntimeError,
                "SetHelper: Could not add the value.");
        }

      };

  }
}

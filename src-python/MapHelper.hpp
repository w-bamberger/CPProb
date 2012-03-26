#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>

namespace cpprob
{
  namespace proby
  {

    template<class MapType>
      class MapHelper
      {

      public:

        typedef typename MapType::key_type key_type;
        typedef typename MapType::mapped_type mapped_type;
        typedef typename MapType::value_type value_type;
        typedef typename MapType::const_iterator const_iterator;
        typedef typename MapType::iterator iterator;

        static void
        del(MapType& m, const key_type& k)
        {
          auto search_it = m.find(k);
          if (search_it != m.end())
            m.erase(search_it);
          else
            PyErr_SetNone(PyExc_KeyError);
        }

        static mapped_type&
        get(MapType& m, const key_type& k)
        {
          auto search_it = m.find(k);
          if (search_it != m.end())
            return search_it->second;
          PyErr_SetNone(PyExc_KeyError);
          boost::python::throw_error_already_set();
          return *static_cast<mapped_type*>(NULL);
        }

        static boost::python::list
        items(const MapType& m)
        {
          boost::python::list t;
          for (auto it = m.begin(); it != m.end(); ++it)
            t.append(boost::python::make_tuple(it->first, it->second));
          return t;
        }

        static boost::python::list
        keys(const MapType& m)
        {
          boost::python::list t;
          for (auto it = m.begin(); it != m.end(); ++it)
            t.append(it->first);
          return t;
        }

        static bool
        contains(const MapType& m, const key_type& k)
        {
          return m.find(k) != m.end();
        }

        static void
        set(MapType& m, const key_type& k, const mapped_type& v)
        {
          m[k] = v;
        }

        static boost::python::list
        values(const MapType& m)
        {
          boost::python::list t;
          for (auto it = m.begin(); it != m.end(); ++it)
            t.append(it->second);
          return t;
        }

      };

  }
}

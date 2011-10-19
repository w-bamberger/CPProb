#include <boost/python/extract.hpp>
#include <boost/python/converter/rvalue_from_python_data.hpp>
#include <list>
#include <stdexcept>

namespace cpprob
{
  namespace proby
  {

    template<class T>
      class ListListConverter
      {

        typedef std::list<T> CppType;

      public:

        // Determine if obj_ptr can be converted in a std::list<T>
        static void*
        convertible(PyObject* obj_ptr)
        {
          if (!PySequence_Check(obj_ptr)
              || !PyObject_HasAttrString(obj_ptr, "__len__"))
            return 0;
          return obj_ptr;
        }

        // Convert obj_ptr into a std::list<T>
        static void
        construct(PyObject* obj_ptr,
            boost::python::converter::rvalue_from_python_stage1_data* data)
        {
          // Create the list in the memory provided by Boost.Python.
          void* storage =
              ((boost::python::converter::rvalue_from_python_storage<CppType>*) (data))->storage.bytes;
          new (storage) CppType();

          // Copy the list content.
          CppType* v = static_cast<CppType*>(storage);
          int l = PySequence_Size(obj_ptr);
          if (l < 0)
            throw std::runtime_error(
                "Proby: Cannot convert Python list to C++ list.");
          for (int i = 0; i < l; i++)
            v->push_back(
                boost::python::extract < T > (PySequence_GetItem(obj_ptr, i)));

          // Save the memory pointer for later use by Boost.Python.
          data->convertible = storage;
        }

        ListListConverter()
        {
          boost::python::converter::registry::push_back(&convertible,
              &construct, boost::python::type_id<CppType>());
        }

      };

  }
}

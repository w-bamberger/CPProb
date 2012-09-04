#include <cpprob/cont/RefVector.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/converter/rvalue_from_python_data.hpp>
#include <stdexcept>

namespace cpprob
{
  namespace proby
  {

    template<class T>
      class ListRefVectorConverter
      {

        typedef ::cpprob::cont::RefVector<T> CppRefListType;

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
              ((boost::python::converter::rvalue_from_python_storage<
                  CppRefListType>*) (data))->storage.bytes;
          new (storage) CppRefListType();

          // Copy the list content.
          CppRefListType* v = static_cast<CppRefListType*>(storage);
          Py_ssize_t l = PySequence_Size(obj_ptr);
          if (l < 0)
            throw std::runtime_error(
                "Proby: Cannot convert Python list to C++ RefVector.");
          v->reserve(l);
          for (Py_ssize_t i = 0; i < l; i++)
            v->push_back(
                boost::python::extract<T&>(PySequence_GetItem(obj_ptr, i)));

          // Save the memory pointer for later use by Boost.Python.
          data->convertible = storage;
        }

        ListRefVectorConverter()
        {
          boost::python::converter::registry::push_back(&convertible,
              &construct, boost::python::type_id<CppRefListType>());
        }

      };

  }
}

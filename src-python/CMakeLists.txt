cmake_minimum_required(VERSION 2.8)
project(Proby)
set(PROBY_VERSION_MAJOR 1)
set(PROBY_VERSION_MINOR 0)
set(PROBY_VERSION "${PROBY_VERSION_MAJOR}.${PROBY_VERSION_MINOR}")

# Dependencies
# ============

find_package(Boost 1.40.0 REQUIRED python)
include_directories(${Boost_INCLUDE_DIRS})
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
find_package(CPProb REQUIRED)
include_directories(${CPPROB_INCLUDE_DIRS})

# Sources
# =======

file(GLOB CPP_SOURCE_FILES *.?pp)
file(GLOB PYTHON_SOURCE_FILES *.py)

# Target
# ======

# The shared library has the name "_proby". The Python package "proby" is
# a directory with an __init__.py module. It loads _proby then.
add_library(_proby SHARED ${CPP_SOURCE_FILES})
set_target_properties(_proby PROPERTIES
    VERSION ${PROBY_VERSION}
    SOVERSION ${PROBY_VERSION_MAJOR})
if(WIN32)
  # On Windows, a DLL that is a Python module should have the suffix .pyd.
  set_target_properties(_proby PROPERTIES SUFFIX ".pyd")
endif(WIN32)

target_link_libraries(_proby ${CPPROB_LIBRARIES})
target_link_libraries(_proby ${Boost_LIBRARIES})
target_link_libraries(_proby ${PYTHON_LIBRARIES})

# Configuration of the compiler
if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -std=c++0x)
elseif(MSVC)
  add_definitions(-DWITHOUT_INITIALIZER_LIST)
endif(CMAKE_COMPILER_IS_GNUCXX)

# Installation
# ============

if(WIN32)
  set(INSTALL_LIB_DIR "${PYTHON_INCLUDE_PATH}/../DLLs/")
  set(INSTALL_PY_DIR "${PYTHON_INCLUDE_PATH}/../Lib/proby")

  # Install the Python package "_proby".
  #
  # On Windows, a DLL is installed with the runtime destination.
  # This avoids that the associated import library (.LIB) is installed.
  # It is not needed here.
  install(TARGETS _proby RUNTIME DESTINATION ${INSTALL_LIB_DIR})

else()
  string(REGEX REPLACE "^([0-9]*).+$" "\\1" PYTHON_MAJOR_VERSION
         ${PYTHONLIBS_VERSION_STRING})
  string(REGEX REPLACE "^[0-9]*\\.([0-9]*).*$" "\\1" PYTHON_MINOR_VERSION
         ${PYTHONLIBS_VERSION_STRING})
  set(INSTALL_LIB_DIR "lib/python${major}${minor}/dist-packages/proby")
  set(INSTALL_PY_DIR "${INSTALL_LIB_DIR}")

  # Install the Python package "_proby".
  install(TARGETS _proby DESTINATION ${INSTALL_LIB_DIR})

endif(WIN32)

# Install the Python package "proby".
install(FILES ${PYTHON_SOURCE_FILES} DESTINATION ${INSTALL_PY_DIR})

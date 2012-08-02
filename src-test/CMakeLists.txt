cmake_minimum_required(VERSION 2.6)
project(CPProb)
set(CPProb_VERSION_MAJOR 1)
set(CPProb_VERSION_MINOR 0)

# Dependencies
if (WIN32)
  set(Boost_USE_STATIC_LIBS        ON)
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME    OFF)
endif(WIN32)
find_package(Boost 1.40.0 REQUIRED program_options unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

# Source
file(GLOB_RECURSE src_files *.?pp)
add_definitions(-DBOOST_TEST_DYN_LINK)
if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=c++0x -Wall -Wextra)
endif(CMAKE_COMPILER_IS_GNUCXX)

# Target
add_executable(cpprobtest ${src_files})
target_link_libraries(cpprobtest cpprob)
target_link_libraries(cpprobtest ${Boost_LIBRARIES})

cmake_minimum_required(VERSION 2.6)
project(CPProb)
set(CPProb_VERSION_MAJOR 1)
set(CPProb_VERSION_MINOR 0)

# Dependencies
find_package(Boost 1.40.0 REQUIRED program_options unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

find_library(CPProb_LIBRARY
             NAMES cpprob libcpprob
             HINTS "../build-lib" "../build-lib/Debug" "../build-lib/Release")

# Source
file(GLOB_RECURSE src_files *.?pp)
add_definitions(-DBOOST_TEST_DYN_LINK)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=c++0x -Wall -Wextra)
endif(CMAKE_COMPILER_IS_GNUCXX)

# Target
if(WIN32)
  # Auto-linking on Windows
  link_directories(${Boost_LIBRARY_DIRS})
  add_executable(cpprobtest ${src_files})
else(WIN32)
  # Linking specific libraries on other platforms
  add_executable(cpprobtest ${src_files})
  target_link_libraries(cpprobtest ${Boost_LIBRARIES})
endif(WIN32)

target_link_libraries(cpprobtest ${CPProb_LIBRARY})

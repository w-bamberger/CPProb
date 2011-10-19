cmake_minimum_required(VERSION 2.6)
project(Proby)
set(CPProb_VERSION_MAJOR 1)
set(CPProb_VERSION_MINOR 0)

# Dependencies
find_package(Boost 1.40.0 REQUIRED python)
include_directories(${Boost_INCLUDE_DIRS})
find_package(PythonInterp)
find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

# Sources
file(GLOB cpp_source_files *.?pp)
file(GLOB python_source_files *.py)

# Target
add_library(proby SHARED ${cpp_source_files})
target_link_libraries(proby cpprob)
target_link_libraries(proby ${Boost_LIBRARIES})
set_target_properties(proby PROPERTIES PREFIX "_")
if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -std=c++0x)
endif(CMAKE_COMPILER_IS_GNUCXX)
        
# Installation
set(destination_path lib/python2.6/dist-packages/proby)
install(FILES ${python_source_files} DESTINATION ${destination_path})
install(TARGETS proby 
        LIBRARY DESTINATION ${destination_path} 
        ARCHIVE DESTINATION ${destination_path})

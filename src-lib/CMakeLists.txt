cmake_minimum_required(VERSION 2.6)
project(CPProb)
set(CPProb_VERSION_MAJOR 1)
set(CPProb_VERSION_MINOR 0)

# Dependencies
find_package(Boost 1.40.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Sources
file(GLOB_RECURSE source_files *.?pp)
file(GLOB main_header_files *.hpp)
file(GLOB cont_header_files cont/*.hpp)
file(GLOB pkgconfig_files *.pc)

# Target
add_library(cpprob_static STATIC ${source_files})
set_target_properties(cpprob_static PROPERTIES
    OUTPUT_NAME "cpprob"
    VERSION ${CPProb_VERSION_MAJOR}.${CPProb_VERSION_MINOR}
    SOVERSION ${CPProb_VERSION_MAJOR})
if(WIN32)
  set_target_properties(cpprob_static PROPERTIES PREFIX "lib")
endif(WIN32)

if(NOT WIN32)
  add_library(cpprob_shared SHARED ${source_files})
  set_target_properties(cpprob_shared PROPERTIES
      OUTPUT_NAME "cpprob"
      VERSION ${CPProb_VERSION_MAJOR}.${CPProb_VERSION_MINOR}
      SOVERSION ${CPProb_VERSION_MAJOR})
endif(NOT WIN32)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=c++0x -Wall -Wextra)
elseif(MSVC)
  add_definitions(-DWITHOUT_INITIALIZER_LIST)
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCPPROB_DEBUG_MODE")
        
# Installation
set(static_lib_destination "lib")
install(TARGETS cpprob_static
        LIBRARY DESTINATION ${static_lib_destination}
        ARCHIVE DESTINATION ${static_lib_destination})
if(NOT WIN32)
  set(shared_lib_destination "lib")
  install(TARGETS cpprob_shared
          LIBRARY DESTINATION ${shared_lib_destination}
          ARCHIVE DESTINATION ${shared_lib_destination})
endif(NOT WIN32)
install(FILES ${main_header_files} DESTINATION include/cpprob)
install(FILES ${cont_header_files} DESTINATION include/cpprob/cont)
install(FILES ${pkgconfig_files} DESTINATION lib/pkgconfig)

# Packaging
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "TGZ;ZIP;DEB;RPM")
set(CPACK_RESOURCE_FILE_LICENSE  
    "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE.txt")
set(CPACK_PACKAGE_NAME "libcpprob-dev")
set(CPACK_PACKAGE_VERSION_MAJOR "${CPProb_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CPProb_VERSION_MINOR}")
set(CPACK_PACKAGE_CONTACT "W. Bamberger <w.bamberger@arcor.de>")
set(CPACK_PACKAGE_DESCRIPTION "Algorithms for probabilistic reasoning")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The CPProb library provides classes and algorithms for various probabilistic reasoning methods. This includes subjective logic and Bayesian networks.")
include (CPack)
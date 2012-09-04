cmake_minimum_required(VERSION 2.6)
project(CPProb)
set(CPPROB_VERSION_MAJOR 1)
set(CPPROB_VERSION_MINOR 0)
set(CPPROB_VERSION "${CPPROB_VERSION_MAJOR}.${CPPROB_VERSION_MINOR}")

# Dependencies
# ============

find_package(Boost 1.40.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Sources
# =======

file(GLOB_RECURSE source_files *.?pp)
file(GLOB_RECURSE header_files RELATIVE "${CMAKE_SOURCE_DIR}" "*.hpp")
file(GLOB pkgconfig_files *.pc)

# Targets
# =======

# For Windows, I take the naming schema of Boost. Static libraries
# with the prefix "lib" (suffix .LIB) and shared libraries (suffixes
# .LIB and .DLL) without a prefix. This is necessary, because the
# suffix .LIB is taken for both kinds of libraries. On Linux, the
# existing naming schema with the suffixes .a / .so is sufficient.
#
# At present though, the code is not ready for DLLs yet. So
# shared libraries are not built on Windows.

add_library(cpprob_static STATIC ${source_files})
set_target_properties(cpprob_static PROPERTIES
    OUTPUT_NAME "cpprob"
    VERSION ${CPPROB_VERSION}
    SOVERSION ${CPPROB_VERSION_MAJOR})
if(WIN32)
  set_target_properties(cpprob_static PROPERTIES PREFIX "lib")
endif(WIN32)

if(NOT WIN32)
  add_library(cpprob_shared SHARED ${source_files})
  set_target_properties(cpprob_shared PROPERTIES
      OUTPUT_NAME "cpprob"
      VERSION ${CPPROB_VERSION}
      SOVERSION ${CPPROB_VERSION_MAJOR})
endif(NOT WIN32)

# Configure the compiler
if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=c++0x -Wall -Wextra)
elseif(MSVC)
  add_definitions(-DWITHOUT_INITIALIZER_LIST)
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCPPROB_DEBUG_MODE")

# Build include directory tree for people who want to use the library
# from the build tree without installation
set(TARGET_HEADER_DIR ${CMAKE_BINARY_DIR}/cpprob)
foreach(header_file ${header_files})
  set(SRC "${CMAKE_SOURCE_DIR}/${header_file}")
  set(DST "${TARGET_HEADER_DIR}/${header_file}")
  add_custom_command(OUTPUT "${DST}"
                     COMMAND ${CMAKE_COMMAND} -E copy "${SRC}" "${DST}"
                     MAIN_DEPENDENCY "${SRC}"
                     COMMENT "-- Generating header ${DST}")
endforeach()
        
# Installation
# ============

export(TARGETS cpprob_static
       FILE "${PROJECT_BINARY_DIR}/CPProbLibraryDepends.cmake")
export(PACKAGE CPProb)

set(INSTALL_INCLUDE_DIR "include")
set(INSTALL_LIB_DIR "lib")
if(WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR "CMake")
else()
  set(INSTALL_CMAKE_DIR "lib/CMake/FooBar")
endif()
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}"
                                        "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")

configure_file(CPProbConfig.cmake.in "${PROJECT_BINARY_DIR}/CPProbConfig.cmake" @ONLY)
configure_file(CPProbConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/CPProbConfigVersion.cmake" @ONLY)
configure_file(CPProbBuildTreeSettings.cmake.in "${PROJECT_BINARY_DIR}/CPProbBuildTreeSettings.cmake" @ONLY)

install(TARGETS cpprob_static
        EXPORT CPProbLibraryDepends
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR})
if(NOT WIN32)
  set(shared_lib_destination "lib")
  install(TARGETS cpprob_shared
          EXPORT CPProbLibraryDepends
          LIBRARY DESTINATION ${INSTALL_LIB_DIR}
          ARCHIVE DESTINATION ${INSTALL_LIB_DIR})
endif(NOT WIN32)
install(DIRECTORY "${TARGET_HEADER_DIR}"
        DESTINATION ${INSTALL_INCLUDE_DIR})
install(FILES "${PROJECT_BINARY_DIR}/CPProbConfig.cmake"
              "${PROJECT_BINARY_DIR}/CPProbConfigVersion.cmake"
        DESTINATION "${INSTALL_CMAKE_DIR}")
install(EXPORT CPProbLibraryDepends
        DESTINATION "${INSTALL_CMAKE_DIR}")
install(FILES ${pkgconfig_files} DESTINATION "lib/pkgconfig")

# Packaging
# =========

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
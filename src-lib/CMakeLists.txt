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

file(GLOB_RECURSE SOURCE_FILES *.?pp)
file(GLOB_RECURSE HEADER_FILES RELATIVE "${CMAKE_SOURCE_DIR}" "*.hpp")
file(GLOB PKGCONFIG_FILES *.pc)

# Targets
# =======

# For Windows, I take the naming schema of Boost. Static libraries
# with the prefix "lib" (suffix .LIB) and shared libraries (suffixes
# .LIB and .DLL) without a prefix. This is necessary, because the
# suffix .LIB is taken for both kinds of libraries. On Linux, the
# existing naming schema with the suffixes .a / .so is sufficient.
add_library(cpprob_static STATIC ${SOURCE_FILES})
set_target_properties(cpprob_static PROPERTIES
    OUTPUT_NAME "cpprob"
    VERSION ${CPPROB_VERSION}
    SOVERSION ${CPPROB_VERSION_MAJOR})
if(WIN32)
  set_target_properties(cpprob_static PROPERTIES PREFIX "lib")
endif(WIN32)

# At present, the code is not ready for DLLs yet. So
# shared libraries are not built on Windows.
if(NOT WIN32)
  add_library(cpprob_shared SHARED ${SOURCE_FILES})
  set_target_properties(cpprob_shared PROPERTIES
      OUTPUT_NAME "cpprob"
      VERSION ${CPPROB_VERSION}
      SOVERSION ${CPPROB_VERSION_MAJOR})
endif(NOT WIN32)

# Configure the compiler.
if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-std=c++0x -Wall -Wextra)
elseif(MSVC)
  add_definitions(-DWITHOUT_INITIALIZER_LIST)
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCPPROB_DEBUG_MODE")

# Build the include directory tree for people, who want to use the
# library from the build tree without installation. For this, every
# header is associated with a target in the binary directory. The
# header needs only be copied to this target. (This only happens
# if it has been changed since the last copying.)
set(TARGET_HEADER_DIR ${CMAKE_BINARY_DIR}/cpprob)
foreach(HEADER_FILE ${HEADER_FILES})
  set(SRC "${CMAKE_SOURCE_DIR}/${HEADER_FILE}")
  set(DST "${TARGET_HEADER_DIR}/${HEADER_FILE}")
  add_custom_command(OUTPUT "${DST}"
                     COMMAND ${CMAKE_COMMAND} -E copy "${SRC}" "${DST}"
                     MAIN_DEPENDENCY "${SRC}"
                     COMMENT "-- Generating header ${DST}")
endforeach()

# Installation
# ============

# Exports the target. This way, the library can directly be imported
# in other CMake projects directly from this build tree. This import
# configuration is also used by find_package.
export(TARGETS cpprob_static
       FILE "${PROJECT_BINARY_DIR}/CPProbLibraryDepends.cmake")
export(PACKAGE CPProb)

# Determine the installation directories.
set(INSTALL_INCLUDE_DIR "include")
set(INSTALL_LIB_DIR "lib")
if(WIN32 AND NOT CYGWIN)
  set(INSTALL_CMAKE_DIR "CMake")
else()
  set(INSTALL_CMAKE_DIR "lib/CMake/FooBar")
endif()
# This is a relative path from the location of the file
# cpprob-config.cmake to the include directory. cpprob-config.cmake
# is found be find_package. Then the include directory can be found
# with a relative path, so that the installation directory can be
# relocated without invalidating it.
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}"
                                        "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")

# Transform the templates for the CMake package configuration files
# to reflect the actual installation directories. These configuration
# files are necessary for find_pacakge. They include the above import
# configuration (of the export command) to configure the library.
configure_file(CPProbConfig.cmake.in
               "${PROJECT_BINARY_DIR}/CPProbConfig.cmake" @ONLY)
configure_file(CPProbConfigVersion.cmake.in
               "${PROJECT_BINARY_DIR}/CPProbConfigVersion.cmake" @ONLY)
configure_file(CPProbBuildTreeSettings.cmake.in
               "${PROJECT_BINARY_DIR}/CPProbBuildTreeSettings.cmake" @ONLY)

# Install the static and shared libraries.
install(TARGETS cpprob_static
        EXPORT CPProbLibraryDepends
        DESTINATION ${INSTALL_LIB_DIR})
if(NOT WIN32)
  set(shared_lib_destination "lib")
  install(TARGETS cpprob_shared
          EXPORT CPProbLibraryDepends
          DESTINATION ${INSTALL_LIB_DIR})
endif(NOT WIN32)

# Install the public headers.
install(DIRECTORY "${TARGET_HEADER_DIR}"
        DESTINATION ${INSTALL_INCLUDE_DIR})

# Install the CMake configuration files
# (including the import configuration)
install(FILES "${PROJECT_BINARY_DIR}/CPProbConfig.cmake"
              "${PROJECT_BINARY_DIR}/CPProbConfigVersion.cmake"
        DESTINATION "${INSTALL_CMAKE_DIR}")
install(EXPORT CPProbLibraryDepends
        DESTINATION "${INSTALL_CMAKE_DIR}")

# Install the pkgconfig configuration files.
install(FILES ${PKGCONFIG_FILES} DESTINATION "lib/pkgconfig")

# Packaging
# =========

include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "TGZ;ZIP;DEB;RPM")
set(CPACK_RESOURCE_FILE_LICENSE
    "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE.txt")
set(CPACK_PACKAGE_NAME "libcpprob-dev")
set(CPACK_PACKAGE_VERSION_MAJOR "${CPPROB_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CPPROB_VERSION_MINOR}")
set(CPACK_PACKAGE_CONTACT "W. Bamberger <w.bamberger@arcor.de>")
set(CPACK_PACKAGE_DESCRIPTION "Algorithms for probabilistic reasoning")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The CPProb library provides classes and algorithms for various probabilistic reasoning methods. This includes subjective logic and Bayesian networks.")
include (CPack)
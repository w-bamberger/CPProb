cmake_minimum_required(VERSION 2.6)
project(CPProb)
set(CPProb_VERSION_MAJOR 1)
set(CPProb_VERSION_MINOR 0)

# Dependencies
find_package(Boost 1.40.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Sources
file(GLOB_RECURSE source_files *.?pp)
file(GLOB_RECURSE header_files *.hpp)

# Target
add_library(cpprob SHARED ${source_files})
set_target_properties(cpprob PROPERTIES
        VERSION ${CPProb_VERSION_MAJOR}.${CPProb_VERSION_MINOR}
        SOVERSION ${CPProb_VERSION_MAJOR})
        
# Installation
install(TARGETS cpprob 
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)
install(FILES ${header_files} DESTINATION include/cpprob)

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
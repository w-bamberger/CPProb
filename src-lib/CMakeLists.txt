# Dependencies
find_package(Boost 1.40.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Source
file(GLOB_RECURSE src_files *.?pp)

# Target
add_library(cpprob SHARED ${src_files})

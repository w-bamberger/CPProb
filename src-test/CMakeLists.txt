# Dependencies
find_package(Boost 1.40.0 REQUIRED program_options)
include_directories(${Boost_INCLUDE_DIRS})

# Source
file(GLOB_RECURSE src_files *.?pp)

# Target
add_executable(cpprobtest ${src_files})
target_link_libraries(cpprobtest cpprob)
target_link_libraries(cpprobtest ${Boost_LIBRARIES})

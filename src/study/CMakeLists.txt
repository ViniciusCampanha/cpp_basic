# Generic test that uses conan libs
add_executable(FindPrimesWithThreads FindPrimesWithThreads.cpp)
target_link_libraries(FindPrimesWithThreads PRIVATE pthread project_options project_warnings)

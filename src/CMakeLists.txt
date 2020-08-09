option(CPP_BASIC_USE_STUDY "Enable compilation of Study" ON)
option(CPP_STARTER_USE_FLTK "Enable compilation of FLTK sample" OFF)

# sdl
if(CPP_BASIC_USE_STUDY)
  message("Using Study")
  add_subdirectory(study)
endif()

# fltk test
if(CPP_STARTER_USE_FLTK)
  message("Using FLTK")
  add_subdirectory(fltk)
endif()

# Generic test that uses conan libs
add_executable(cpp_basic main.cpp)
target_link_libraries(cpp_basic PRIVATE pthread project_options project_warnings)

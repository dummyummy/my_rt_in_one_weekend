cmake_minimum_required(VERSION 3.13)

project(tracer VERSION 1.0.0)

add_library(target_compile_flags INTERFACE)
target_compile_features(target_compile_flags INTERFACE cxx_std_17)
target_compile_options(target_compile_flags INTERFACE
                       $<$<CONFIG:DEBUG>:-O0>
                       $<$<CONFIG:DEBUG>:-g>
                       $<$<CONFIG:RELEASE>:-O2>
                       )

# do not need X11
target_compile_definitions(target_compile_flags INTERFACE cimg_display=0 cimg_use_png)

find_package(PNG REQUIRED)

# add_subdirectory(lib)

add_executable(tracer main.cpp)

target_include_directories(tracer PUBLIC
                           ${PNG_INCLUDE_DIRS}
                           inc)

target_link_libraries(tracer PUBLIC
                      ${PNG_LIBRARIES}
                      target_compile_flags)
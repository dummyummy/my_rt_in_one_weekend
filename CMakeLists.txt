cmake_minimum_required(VERSION 3.13)

project(tracer VERSION 1.0.0)

add_library(target_compile_flags INTERFACE)
target_compile_features(target_compile_flags INTERFACE cxx_std_17)
if(WIN32)
    target_compile_options(target_compile_flags INTERFACE
                           $<$<CONFIG:DEBUG>:/Od>
                           $<$<CONFIG:DEBUG>:/DEBUG>
                           $<$<CONFIG:RELEASE>:/O2>)
else()
    target_compile_options(target_compile_flags INTERFACE
                           $<$<CONFIG:DEBUG>:-O0>
                           $<$<CONFIG:DEBUG>:-g>
                           $<$<CONFIG:RELEASE>:-O2>)
endif()

# do not need X11
target_compile_definitions(target_compile_flags INTERFACE cimg_display=0 cimg_use_png)
if(WIN32)
    set(PNG_INCLUDE_DIRS "C:/Program Files (x86)/libpng/include")
    find_library(PNG_LIBRARIES NAMES libpng16 libpng16d PATHS "C:/Program Files (x86)/libpng/lib")
    message("libpng found at ${PNG_LIBRARIES}")
else()
    find_package(PNG REQUIRED)
endif()

# add_subdirectory(inc)

add_executable(tracer main.cpp)

target_include_directories(tracer PUBLIC
                           ${PNG_INCLUDE_DIRS}
                           inc)

target_link_libraries(tracer PUBLIC
                      ${PNG_LIBRARIES}
                      target_compile_flags)
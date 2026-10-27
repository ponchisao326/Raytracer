cmake_minimum_required(VERSION 3.20)
project(Raytracer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

file(GLOB SOURCES src/*/*.cpp)
add_library(core STATIC ${SOURCES})
target_include_directories(core PUBLIC src)

add_executable(raytracer src/main.cpp)
target_link_libraries(raytracer core)

enable_testing()

add_executable(image_tests tests/image_tests.cpp)
target_link_libraries(image_tests core)
add_test(NAME image_tests COMMAND image_tests)

add_executable(render_tests tests/render_tests.cpp)
target_link_libraries(render_tests core)
add_test(NAME render_tests COMMAND render_tests)

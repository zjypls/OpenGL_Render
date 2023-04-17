cmake_minimum_required(VERSION 3.20)

project(OpenGLLib)

file(GLOB_RECURSE sources *.cpp *.h)
add_compile_definitions(Z_SHADER_PATH="${CMAKE_SOURCE_DIR}/Assets/Shaders/")
add_library(OpenGLLib STATIC ${sources})

target_include_directories(OpenGLLib PUBLIC dep ../imgui)
target_compile_definitions(OpenGLLib PUBLIC Z_SHADER_PATH)
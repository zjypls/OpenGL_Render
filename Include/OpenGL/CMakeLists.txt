cmake_minimum_required(VERSION 3.20)

project(OpenGLLib)

file(GLOB_RECURSE sources *.cpp *.h)

add_library(OpenGLLib STATIC ${sources})

target_include_directories(OpenGLLib PUBLIC dep ../imgui)
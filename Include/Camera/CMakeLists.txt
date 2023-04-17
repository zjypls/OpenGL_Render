cmake_minimum_required(VERSION 3.20)

project(Camera)

add_library(Camera STATIC
    Camera.cpp
    Camera.h)
target_include_directories(Camera PUBLIC ../)
target_link_libraries(Camera PUBLIC glad)
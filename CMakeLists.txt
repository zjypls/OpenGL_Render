cmake_minimum_required(VERSION 3.25)
project(OpenGL_Render)

#set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Lib)
option(Z_USE_OPENGL ON)
set(CMAKE_CXX_STANDARD 17)
link_directories(${CMAKE_SOURCE_DIR}/Lib)
include_directories(${CMAKE_SOURCE_DIR}/Include
        ${CMAKE_SOURCE_DIR}/Include/glad
        ${CMAKE_SOURCE_DIR}/Include/glfw/include
        ${CMAKE_SOURCE_DIR}/OpenGL dep)

add_subdirectory(Include)

add_subdirectory(src)

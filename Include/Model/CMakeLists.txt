cmake_minimum_required(VERSION 3.20)


project(Model)
include_directories(../assimp/include dep)
add_library(Model STATIC
        Model.cpp Model.h)
add_compile_definitions(Z_MODULE_SOURCE_DIR="${CMAKE_SOURCE_DIR}/Assets/Model/")
target_include_directories(Model PUBLIC dep)
target_link_libraries(Model PUBLIC assimp)
target_compile_definitions(Model PUBLIC Z_MODULE_SOURCE_DIR)
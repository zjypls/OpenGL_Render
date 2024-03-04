cmake_minimum_required(VERSION 3.20)

project(pbr_Gbuffer)

link_directories(${CMAKE_SOURCE_DIR}/Lib)
add_executable(${PROJECT_NAME} main.cpp ModelPBR.cpp ModelPBR.h)
target_include_directories(${PROJECT_NAME} PUBLIC dep)
target_link_libraries(${PROJECT_NAME} glfw glad OpenGLLib Model Camera ImGui)
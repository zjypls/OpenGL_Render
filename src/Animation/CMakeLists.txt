cmake_minimum_required(VERSION 3.20)


PROJECT(animationPlay)

file(GLOB src CONFIGURE_DEPENDS *.cpp *.h)


add_executable(${PROJECT_NAME} ${src})
target_compile_definitions(${PROJECT_NAME} PUBLIC Z_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME} glfw glad OpenGLLib Model Camera ImGui)
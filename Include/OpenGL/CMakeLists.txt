cmake_minimum_required(VERSION 3.20)

project(OpenGLLib)

file(GLOB sources *.cpp *.h)
add_compile_definitions(Z_SHADER_PATH="${CMAKE_SOURCE_DIR}/Assets/Shaders/")
add_compile_definitions(Z_TEXTURE_PATH="${CMAKE_SOURCE_DIR}/Assets/Textures/")
add_library(OpenGLLib STATIC ${sources} ../ImGuizmo/ImGuizmo.cpp ../ImGuizmo/ImGuizmo.h)
target_link_libraries(OpenGLLib PUBLIC glfw glad ImGui)
target_include_directories(OpenGLLib PUBLIC dep ../imgui)
target_compile_definitions(OpenGLLib PUBLIC Z_SHADER_PATH Z_TEXTURE_PATH)

if(CMAKE_HOST_WIN32)
    target_compile_definitions(OpenGLLib PUBLIC Z_PLATFORM_WINDOWS)
elseif(CMAKE_HOST_LINUX)
    target_compile_definitions(OpenGLLib PUBLIC Z_PLATFORM_LINUX)
endif()
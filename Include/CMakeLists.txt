cmake_minimum_required(VERSION 3.20)


add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(assimp)
add_subdirectory(imgui)
add_subdirectory(OpenGL)
add_dependencies(OpenGLLib glad GLFW assimp ImGui)
add_subdirectory(Model)
add_subdirectory(Camera)
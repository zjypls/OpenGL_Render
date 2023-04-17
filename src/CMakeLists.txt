cmake_minimum_required(VERSION 3.20)

add_subdirectory(PBR)
add_subdirectory(RayTracingOneWeek)
#set start project for visual studio
if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PBR)
endif ()
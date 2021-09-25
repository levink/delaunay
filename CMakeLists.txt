cmake_minimum_required(VERSION 3.20)
project(delone)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(lib/glfw-3.3.4)
add_subdirectory(lib/glad)

add_executable(delone
        src/camera/camera.cpp
        src/model/circle.cpp
        src/model/color.cpp
        src/platform/loader.cpp
        src/platform/log.cpp
        src/shader/shader.cpp
        src/shader/shaderBase.cpp
        src/shader/shaderSource.cpp
        src/main.cpp
        src/render.cpp
        src/ui.cpp)
target_precompile_headers(delone PUBLIC
        src/pch.h
        include/glad/glad.h
        lib/glfw-3.3.4/include/GLFW/glfw3.h)
target_link_libraries(delone glad glfw)


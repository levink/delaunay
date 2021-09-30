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
        src/model/face.cpp
        src/model/line.cpp
        src/platform/loader.cpp
        src/platform/log.cpp
        src/scene/drawBatch.cpp
        src/scene/scene.cpp
        src/shader/shader.cpp
        src/shader/shaderBase.cpp
        src/shader/shaderSource.cpp
        src/ui/ui.cpp
        src/main.cpp
        src/render.cpp)
target_precompile_headers(delone PUBLIC src/pch.h)
target_link_libraries(delone glad glfw)


cmake_minimum_required(VERSION 3.20)
project(delone)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(lib/glfw-3.3.4)

add_subdirectory(lib/glad)

add_executable(delone
        src/model/delaunay.cpp
        src/model/drawBatch.cpp
        src/scene/camera.cpp
        src/scene/scene.cpp

        src/shader/shader.cpp
        src/shader/shaderBase.cpp
        src/shader/shaderLoader.cpp
        src/shader/shaderLog.cpp
        src/shader/shaderSource.cpp
        
        src/ui/ui.cpp
        src/view/circle.cpp
        src/view/color.cpp
        src/view/face.cpp
        src/view/line.cpp
        src/render.cpp
        src/main.cpp)

target_precompile_headers(delone PRIVATE src/pch.h)

target_link_libraries(delone glad glfw)


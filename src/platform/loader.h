#pragma once
#include <vector>
#include <src/shader/shaderSource.h>

/* Классы для загрузки ресурсов */
namespace files {
    static const char* circleVert = "../data/shaders/circle.vert";
    static const char* circleFrag = "../data/shaders/circle.frag";
}

class Platform {
public:
    Platform() = default;
};

class ShaderLoader {
    std::string getShader(const char* fileName);
public:
    ShaderLoader() = default;
    explicit ShaderLoader(Platform& platform) { }
    ShaderSource load(const char* vertex, const char* fragment);
};

#pragma once
#include "src/shader/shaderSource.h"

/* Классы для загрузки ресурсов */
namespace files {
    static const char* circle = "../../data/shaders/circle.glsl";
    static const char* line = "../../data/shaders/line.glsl";
}

class Platform {
public:
    Platform() = default;
};

class ShaderLoader {
    static std::string getShaderText(const char* fileName);
public:
    ShaderLoader() = default;
    explicit ShaderLoader(Platform& platform) { }
    ShaderSource load(const char* path) const;
};

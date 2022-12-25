#pragma once
#include "shader/shaderSource.h"

namespace files {
    static const char* circle   = "../../data/shaders/circle.glsl";
    static const char* line     = "../../data/shaders/line.glsl";
}

class ShaderLoader {
    static std::string getShaderText(const char* fileName);
public:
    ShaderLoader() = default;
    ShaderSource load(const char* path) const;
};

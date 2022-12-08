#pragma once
#include "pch.h"

struct ShaderSource {
    std::string vertex;
    std::string fragment;
    ShaderSource() = default;
    ShaderSource(const ShaderSource& right);
    ShaderSource(ShaderSource&& right) noexcept;
    ShaderSource& operator=(const ShaderSource& right);
    ShaderSource& operator=(ShaderSource&& right) noexcept;
};

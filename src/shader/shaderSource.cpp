//
// Created by Konst on 22.09.2021.
//

#include "shaderSource.h"

ShaderSource::ShaderSource(const ShaderSource &right) {
    vertex = right.vertex;
    fragment = right.fragment;
}
ShaderSource::ShaderSource(ShaderSource &&right) noexcept {
    vertex = std::move(right.vertex);
    fragment = std::move(right.fragment);
}
ShaderSource &ShaderSource::operator=(const ShaderSource &right) {
    if (this == &right) {
        return *this;
    }

    vertex = right.vertex;
    fragment = right.fragment;
    return *this;
}
ShaderSource &ShaderSource::operator=(ShaderSource &&right) noexcept {
    if (this == &right) {
        return *this;
    }

    vertex = std::move(right.vertex);
    fragment = std::move(right.fragment);
    return *this;
}
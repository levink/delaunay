//
// Created by Konst on 22.09.2021.
//
#pragma once
#include <glad/glad.h>

struct AttributeInfo {
    int dimension;
    int offsetBytes;
    GLenum dataType;
    int elementSize;
};

struct Attribute {
    constexpr static const AttributeInfo vec2 = AttributeInfo{2, 0, GL_FLOAT, 0 };
};

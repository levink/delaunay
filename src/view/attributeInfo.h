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

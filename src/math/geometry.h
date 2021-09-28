#pragma once
#include "src/pch.h"

struct Triangle {
    glm::vec2 a, b, c;
};

struct Layer {
    std::vector<Triangle> triangles;
};

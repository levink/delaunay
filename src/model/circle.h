#pragma once
#include "src/pch.h"
#include "face.h"

struct CircleVertex {
    glm::vec2 center;
    glm::vec2 offset;
    float radius;
    float fill;
    CircleVertex();
    CircleVertex(float x, float y,
                 float offsetX, float offsetY,
                 float radius,
                 float fill);
    void move(float x, float y);
};

struct CircleMesh {
    std::vector<CircleVertex> vertex;
    std::vector<Face> face;
    glm::vec3 color;
    CircleMesh(glm::vec2 center, float radius, bool filled, glm::vec3 color);
};
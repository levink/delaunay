#pragma once
#include "pch.h"
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
};

struct CircleMesh {
    std::vector<CircleVertex> vertex;
    std::vector<Face> face;
    glm::vec3 color;
    CircleMesh() = default;
    CircleMesh(glm::vec2 center, float radius, bool filled, glm::vec3 color);
    void setPosition(const glm::vec2& position);

    static CircleMesh createPoint(const glm::vec2& point, bool selected);
};
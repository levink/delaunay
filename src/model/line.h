#pragma once
#include "src/pch.h"
#include "face.h"

struct LineVertex {
    glm::vec2 position;
    glm::vec2 e1;
    glm::vec2 e2;
    glm::vec2 offset;
    LineVertex();
    LineVertex(glm::vec2 position,
               glm::vec2 e1,
               glm::vec2 e2,
               glm::vec2 offset);
};

struct LineMesh {
    std::vector<LineVertex> vertex;
    std::vector<Face> face;
    glm::vec3 color;
    float width;
    int count() const { return (int)(face.size() * 3u); }
    const Face* data() const { return face.data(); }
};

struct Line {
    std::vector<glm::vec2> points;
    LineMesh mesh;

    void appendPoint(float x, float y);
};
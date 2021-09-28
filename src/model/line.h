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

struct Line {
    std::vector<glm::vec2> points;
    std::vector<LineVertex> vertex;
    std::vector<Face> face;
    void addPoint(float x, float y);
};


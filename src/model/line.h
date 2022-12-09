#pragma once
#include "pch.h"
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
    void set(const glm::vec2& position,
             const glm::vec2& e1,
             const glm::vec2& e2);
};

struct LineMesh {
    std::vector<LineVertex> vertex;
    std::vector<Face> face;
    LineMesh();
    LineMesh(const glm::vec2& start, const glm::vec2& end);
    int count() const { return (int)(face.size() * 3u); }
    const Face* faceData() const { return face.data(); }
    void move(const glm::vec2& start, const glm::vec2& end);
};
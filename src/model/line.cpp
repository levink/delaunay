#include "line.h"

LineVertex::LineVertex() = default;
LineVertex::LineVertex(glm::vec2 position, glm::vec2 e1, glm::vec2 e2, glm::vec2 offset) :
    position(position),
    e1(e1),
    e2(e2),
    offset(offset) { }

void Line::addPoint(float x, float y) {

    points.emplace_back(x, y);

    if (points.size() < 2) {
        return;
    }

    auto& a = points[points.size() - 2];
    auto& b = points[points.size() - 1];

    auto vertexSize = vertex.size();
    vertex.reserve(vertexSize + 4);
    vertex.emplace_back(LineVertex(a, a, b, {-1, -1}));
    vertex.emplace_back(LineVertex(b, a, b, {-1, +1}));
    vertex.emplace_back(LineVertex(b, a, b, {+1, +1}));
    vertex.emplace_back(LineVertex(a, a, b, {+1, -1}));

    face.reserve(face.size() + 2);
    face.emplace_back(Face(vertexSize + 0,vertexSize + 1,vertexSize + 2));
    face.emplace_back(Face(vertexSize + 2,vertexSize + 3,vertexSize + 0));
}

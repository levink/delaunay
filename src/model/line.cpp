#include "line.h"

LineVertex::LineVertex() = default;
LineVertex::LineVertex(glm::vec2 position, glm::vec2 e1, glm::vec2 e2, glm::vec2 offset) :
    position(position),
    e1(e1),
    e2(e2),
    offset(offset) { }

void LineVertex::set(
        const glm::vec2& position,
        const glm::vec2& e1,
        const glm::vec2& e2) {
    this->position = position;
    this->e1 = e1;
    this->e2 = e2;
}

LineMesh LineMesh::create(const glm::vec2 &start, const glm::vec2 &end, const glm::vec3& color, float width) {

    LineMesh mesh;

    mesh.vertex.reserve(4);
    mesh.vertex.emplace_back(LineVertex(start,  start, end, {-1, -1}));
    mesh.vertex.emplace_back(LineVertex(end,    start, end, {-1, +1}));
    mesh.vertex.emplace_back(LineVertex(end,    start, end, {+1, +1}));
    mesh.vertex.emplace_back(LineVertex(start,  start, end, {+1, -1}));

    mesh.face.reserve(mesh.face.size() + 2);
    mesh.face.emplace_back(Face(0, 1, 2));
    mesh.face.emplace_back(Face(2, 3, 0));

    mesh.color = color;
    mesh.width = width;

    return mesh;
}

void LineMesh::move(const glm::vec2 &start, const glm::vec2 &end) {
    vertex[0].set(start, start, end);
    vertex[1].set(end, start, end);
    vertex[2].set(end, start, end);
    vertex[3].set(start, start, end);
}

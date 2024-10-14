#include "line.h"
#include "model/color.h"

LineVertex::LineVertex() = default;
LineVertex::LineVertex(glm::vec2 position, glm::vec2 e1, glm::vec2 e2, glm::vec2 offset) :
    position(position),
    e1(e1),
    e2(e2),
    offset(offset) { }
void LineVertex::set(const glm::vec2& position, const glm::vec2& e1, const glm::vec2& e2) {
    this->position = position;
    this->e1 = e1;
    this->e2 = e2;
}

LineMesh::LineMesh() {
    glm::vec2 start(0, 0);
    glm::vec2 end(0, 0);

    vertex.reserve(4);
    vertex.emplace_back(LineVertex(start, start, end, { -1, -1 }));
    vertex.emplace_back(LineVertex(end, start, end, { -1, +1 }));
    vertex.emplace_back(LineVertex(end, start, end, { +1, +1 }));
    vertex.emplace_back(LineVertex(start, start, end, { +1, -1 }));

    face.reserve(2);
    face.emplace_back(Face(0, 1, 2));
    face.emplace_back(Face(2, 3, 0));
}
LineMesh::LineMesh(const glm::vec2& start, const glm::vec2& end) {
    vertex.reserve(4);
    vertex.emplace_back(LineVertex(start, start, end, { -1, -1 }));
    vertex.emplace_back(LineVertex(end, start, end, { -1, +1 }));
    vertex.emplace_back(LineVertex(end, start, end, { +1, +1 }));
    vertex.emplace_back(LineVertex(start, start, end, { +1, -1 }));

    face.reserve(2);
    face.emplace_back(Face(0, 1, 2));
    face.emplace_back(Face(2, 3, 0));
}
void LineMesh::movePosition(const glm::vec2 &start, const glm::vec2 &end) {
    vertex[0].set(start, start, end);
    vertex[1].set(end, start, end);
    vertex[2].set(end, start, end);
    vertex[3].set(start, start, end);
}

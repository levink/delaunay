#include "circle.h"
#include "model/color.h"

CircleVertex::CircleVertex() 
    : center(0,0), offset(0,0), radius(0.f), fill(0.f) { }

CircleVertex::CircleVertex(float x, float y, float offsetX, float offsetY, float radius, float fill):
        center(x,y), offset(offsetX, offsetY), radius(radius), fill(fill) { }

CircleMesh::CircleMesh(glm::vec2 center, float radius, bool filled, glm::vec3 color) {
    float x = center.x;
    float y = center.y;
    float fill = filled ? 1.f : 0.f;

    vertex.resize(4);
    vertex[0] = CircleVertex {x, y, -1, -1, radius, fill };
    vertex[1] = CircleVertex {x, y, +1, -1, radius, fill };
    vertex[2] = CircleVertex {x, y, +1, +1, radius, fill };
    vertex[3] = CircleVertex {x, y, -1, +1, radius, fill };

    face.resize(2);
    face[0] = Face(0,1,2);
    face[1] = Face(2,3,0);

    this->color = color;
}

void CircleMesh::setPosition(const glm::vec2& position) {
    if (vertex.size() < 4){
        vertex.resize(4);
    }
    vertex[0].center = position;
    vertex[1].center = position;
    vertex[2].center = position;
    vertex[3].center = position;
}

CircleMesh CircleMesh::createPoint(const glm::vec2& point) {
    constexpr bool filled = true;
    return CircleMesh(point, 7.f, filled, Color::teal);
}

CircleMesh CircleMesh::createPointSelected(const glm::vec2& point) {
    constexpr bool filled = true;
    return CircleMesh(point, 7.f, filled, Color::orange);
}

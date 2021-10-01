
#include "circle.h"

CircleVertex::CircleVertex():
    center(0,0), offset(0,0), radius(0.f), fill(0.f) { }

CircleVertex::CircleVertex(float x, float y, float offsetX, float offsetY, float radius, float fill):
        center(x,y), offset(offsetX, offsetY), radius(radius), fill(fill) { }

void CircleVertex::move(float x, float y) {
    center.x = x;
    center.y = y;
}

CircleMesh::CircleMesh(glm::vec2 center, float radius, bool filled, glm::vec3 color) {
    float fill = filled ? 1.f : 0.f;

    vertex.resize(4);
    vertex[0] = CircleVertex {center.x, center.y, -1, -1, radius, fill };
    vertex[1] = CircleVertex {center.x, center.y, +1, -1, radius, fill };
    vertex[2] = CircleVertex {center.x, center.y, +1, +1, radius, fill };
    vertex[3] = CircleVertex {center.x, center.y, -1, +1, radius, fill };

    face.resize(2);
    face[0] = Face(0,1,2);
    face[1] = Face(2,3,0);

    this->color = color;
}

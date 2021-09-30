
#include "circle.h"

CircleVertex::CircleVertex():
    position{0}, center{0}, radius(0), fill(0.f) { }

CircleVertex::CircleVertex(float x, float y, float cx, float cy, float r, float fill):
    position{x, y}, center{cx, cy}, radius(r), fill(fill) { }

CircleMesh::CircleMesh(float x, float y, float r, bool filled) {
    float fill = filled ? 1.f : 0.f;

    vertex[0] = CircleVertex(x - r, y - r, x, y, r, fill);
    vertex[1] = CircleVertex(x - r, y + r, x, y, r, fill);
    vertex[2] = CircleVertex(x + r, y + r, x, y, r, fill);
    vertex[3] = CircleVertex(x + r, y - r, x, y, r, fill);

    face[0] = Face(0,1,2);
    face[1] = Face(2,3,0);
}
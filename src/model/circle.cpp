#include "circle.h"
#include "color.h"

CircleModel::CircleModel(float x, float y, float r) {
    glm::vec2 positions[4] = {
        glm::vec2(x - r, y - r),
        glm::vec2(x - r, y + r),
        glm::vec2(x + r, y + r),
        glm::vec2(x + r, y - r),
    };

    vertices.reserve(4);
    for(auto& pos : positions) {
        CircleVertex vertex {};
        vertex.position[0] = pos.x;
        vertex.position[1] = pos.y;
        vertex.center[0] = x;
        vertex.center[1] = y;
        vertex.radius = r;
        vertex.fill = 0.0;
        vertex.color[0] = Color::teal.r;
        vertex.color[1] = Color::teal.g;
        vertex.color[2] = Color::teal.b;
        vertices.push_back(vertex);
    }
    faces = {
        {0,1,2},
        {2,3,0},
    };
}

void CircleModel::fill(bool value) {
    for(auto& v : vertices) {
        v.fill = value ? 1.0 : 0.0;
    }
}

void CircleModel::color(const glm::vec3 &color) {
    for(auto& v : vertices) {
        v.color[0] = color.r;
        v.color[1] = color.g;
        v.color[2] = color.b;
    }
}



//
// Created by Konst on 22.09.2021.
//
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



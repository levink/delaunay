//
// Created by Konst on 22.09.2021.
//
#include "circle.h"
#include "color.h"

CircleModel::CircleModel(float x, float y, float r) {
    vertex = {
        glm::vec2(x - r, y - r),
        glm::vec2(x - r, y + r),
        glm::vec2(x + r, y + r),
        glm::vec2(x + r, y - r),
    };
    faces = {
        {0, 1,2},
        {2, 3,0},
    };
    radius = r;
    center.x = x;
    center.y = y;
    color = &Color::teal;
}

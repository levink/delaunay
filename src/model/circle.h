//
// Created by Konst on 22.09.2021.
//
#pragma once
#include <glm/glm.hpp>
#include <vector>

struct CircleModel {
    std::vector<glm::vec2> vertex;
    std::vector<glm::u16vec3> faces;
    glm::vec2 center;
    float radius;
    const glm::vec3* color;
};

//
// Created by Konst on 22.09.2021.
//
#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    glm::ivec2 viewSize;
    glm::mat4 Ortho;
    void reshape(int w, int h);
};

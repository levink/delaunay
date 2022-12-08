#pragma once
#include "pch.h"

class Camera {
public:
    glm::ivec2 viewSize;
    glm::mat4 Ortho;
    void reshape(int w, int h);
};

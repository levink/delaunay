#pragma once
#include <GLFW/glfw3.h>
#include "pch.h"

struct DrawBatch {
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLsizei count = 0;
    void init(float w, float h);
    void destroy();
};

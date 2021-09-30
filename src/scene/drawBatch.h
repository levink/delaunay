#pragma once
#include <GLFW/glfw3.h>
#include "src/pch.h"

struct DrawBatch {
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLuint count = 0;
    void destroy();
};

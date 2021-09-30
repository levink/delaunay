#include <glad/glad.h>
#include "drawBatch.h"

void DrawBatch::destroy() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    vbo = 0;
    ibo = 0;
}

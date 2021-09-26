#include "camera.h"
#include <glad/glad.h>

void Camera::reshape(int w, int h) {
    viewSize.x = std::clamp(w, 0,4096);
    viewSize.y = std::clamp(h, 0,4096);
    Ortho = glm::ortho(
            0.f, static_cast<float>(viewSize.x),
            0.f, static_cast<float>(viewSize.y));
    glViewport(0, 0, viewSize.x, viewSize.y);
}

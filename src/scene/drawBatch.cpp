#include <glad/glad.h>
#include <random>
#include "drawBatch.h"
#include "scene/scene.h"

void DrawBatch::init(float w, float h) {
    std::random_device rd;
    std::mt19937 mt(rd());

    constexpr size_t points_count = 10;
    std::vector<CircleVertex> vertex;
    std::vector<Face> face;

    vertex.reserve(4 * points_count);
    face.reserve(2 * points_count);
    for (int i = 0; i < points_count; i++) {
        std::uniform_real_distribution<float> getRandomX(0.f, w);
        std::uniform_real_distribution<float> getRandomY(0.f, h);
        std::uniform_real_distribution<float> getRandomRadius(50.f, 200.f);

        float x = getRandomX(mt);
        float y = getRandomY(mt);
        float r = getRandomRadius(mt);
        float fill = 0.f;

        vertex.emplace_back(CircleVertex{ x, y, -1, -1, r, fill });
        vertex.emplace_back(CircleVertex{ x, y, +1, -1, r, fill });
        vertex.emplace_back(CircleVertex{ x, y, +1, +1, r, fill });
        vertex.emplace_back(CircleVertex{ x, y, -1, +1, r, fill });

        const uint16_t offset = i * 4;
        face.emplace_back(Face{ offset + 0u, offset + 1u, offset + 2u });
        face.emplace_back(Face{ offset + 2u, offset + 3u, offset + 0u });
    }

    GLuint vbo;
    auto vertexData = vertex.data();
    auto vertexSize = (long)vertex.size() * (long)sizeof(CircleVertex);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);

    GLuint ibo;
    auto faceData = face.data();
    auto faceSize = (long)face.size() * (long)sizeof(Face);
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceSize, faceData, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    this->vbo = vbo;
    this->ibo = ibo;
    this->count = face.size() * 6;
}

void DrawBatch::destroy() {
    if (vbo > 0) glDeleteBuffers(1, &vbo);
    if (ibo > 0) glDeleteBuffers(1, &ibo);
    vbo = 0;
    ibo = 0;
}

#include <random>
#include "circle.h"

CircleVertex::CircleVertex():
    position{0}, center{0}, radius(0), fill(0.f) { }

CircleVertex::CircleVertex(float x, float y, float cx, float cy, float r, float fill):
    position{x, y}, center{cx, cy}, radius(r), fill(fill) { }

CircleModel::CircleModel(float x, float y, float r, bool filled) {
    float fill = filled ? 1.f : 0.f;

    vertex[0] = CircleVertex(x - r, y - r, x, y, r, fill);
    vertex[1] = CircleVertex(x - r, y + r, x, y, r, fill);
    vertex[2] = CircleVertex(x + r, y + r, x, y, r, fill);
    vertex[3] = CircleVertex(x + r, y - r, x, y, r, fill);

    face[0] = Face(0,1,2);
    face[1] = Face(2,3,0);
}


CircleBatch::CircleBatch(const glm::vec2& viewSize) {
    std::random_device rd;
    std::mt19937 mt(rd());

    int count = 10;
    vertex.reserve(4 * count);
    face.reserve(2 * count);

    for(int i = 0; i < count; i++) {

        std::uniform_real_distribution<float> getRandomX(0.f, (float)viewSize.x);
        std::uniform_real_distribution<float> getRandomY(0.f, (float)viewSize.y);
        std::uniform_real_distribution<float> getRandomRadius(50.f, 120.f);

        float x = getRandomX(mt);
        float y = getRandomY(mt);
        float r = getRandomRadius(mt);
        float fill = (i % 5) ? 0.f : 1.f;

        int offset= i * 4;
        vertex.emplace_back(x - r, y - r, x, y, r, fill);
        vertex.emplace_back(x - r, y + r, x, y, r, fill);
        vertex.emplace_back(x + r, y + r, x, y, r, fill);
        vertex.emplace_back(x + r, y - r, x, y, r, fill);

        face.emplace_back(offset + 0,offset + 1,offset + 2);
        face.emplace_back(offset + 2,offset + 3,offset + 0);
    }
}
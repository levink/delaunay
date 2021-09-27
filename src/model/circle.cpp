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


CircleItems::CircleItems(const glm::vec2& viewSize) {
    std::random_device rd;
    std::mt19937 mt(rd());

    for(int i=0; i < 10; i++) {

        std::uniform_real_distribution<float> getRandomX(0.f, (float)viewSize.x);
        std::uniform_real_distribution<float> getRandomY(0.f, (float)viewSize.y);
        std::uniform_real_distribution<float> getRandomRadius(50.f, 120.f);

        float x = getRandomX(mt);
        float y = getRandomY(mt);
        float r = getRandomRadius(mt);
        auto circle = CircleModel(x, y, r, !(i % 4));

        auto offset = static_cast<glm::uint16>(vertex.size());
        vertex.insert(std::end(vertex), std::begin(circle.vertex), std::end(circle.vertex));

        for(auto faceCopy : circle.face) {
            faceCopy.a += offset;
            faceCopy.b += offset;
            faceCopy.c += offset;
            face.push_back(faceCopy);
        }
    }
}
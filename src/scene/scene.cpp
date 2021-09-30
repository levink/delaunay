#include "scene.h"

void Scene::addPoint(float x, float y) {
    points.emplace_back(x, y);
    pointsMesh.emplace_back(x, y, 4.f, true);

    if (points.size() == 3) {
        edges.push_back({0, 1, 0, -1});
        edges.push_back({1, 2, 0, -1});
        edges.push_back({2, 0, 0, -1});
        triangles.push_back({0, 1, 2, 0, 1, 2});

        for(auto& e : edges) {
            auto& a = points[e.v1];
            auto& b = points[e.v2];

            auto vertexSize = lineMesh.vertex.size();
            lineMesh.vertex.reserve(vertexSize + 4);
            lineMesh.vertex.emplace_back(LineVertex(a, a, b, {-1, -1}));
            lineMesh.vertex.emplace_back(LineVertex(b, a, b, {-1, +1}));
            lineMesh.vertex.emplace_back(LineVertex(b, a, b, {+1, +1}));
            lineMesh.vertex.emplace_back(LineVertex(a, a, b, {+1, -1}));

            lineMesh.face.reserve(lineMesh.face.size() + 2);
            lineMesh.face.emplace_back(Face(vertexSize + 0,vertexSize + 1,vertexSize + 2));
            lineMesh.face.emplace_back(Face(vertexSize + 2,vertexSize + 3,vertexSize + 0));
        }
    }
}

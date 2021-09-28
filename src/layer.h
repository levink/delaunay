#pragma once
#include "src/pch.h"

struct Layer {

    struct Edge {
        int v1, v2;
        int t1, t2;
    };

    struct Triangle {
        int v1, v2, v3;
        int e1, e2, e3;
    };

    //model
    std::vector<glm::vec2> points;
    std::vector<Edge> edges;
    std::vector<Triangle> triangles;

    //view
    LineMesh lineMesh;
    std::vector<CircleMesh> pointsMesh;

    void addPoint(float x, float y) {
        points.emplace_back(x, y);

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

            for(auto& p: points) {
                pointsMesh.emplace_back(p.x, p.y, 3.f, true);
            }
        }
    }
};

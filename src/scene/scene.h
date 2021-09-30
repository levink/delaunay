#pragma once
#include "src/pch.h"
#include "../model/line.h"
#include "../model/circle.h"

struct Scene {

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

    void addPoint(float x, float y);
};

#pragma once
#include "src/pch.h"
#include "../model/line.h"
#include "../model/circle.h"

struct Scene {

    struct Circle {
        glm::vec2 center;
        float radius;
        Circle();
        Circle(float x, float y, float radius);
        bool contains(float x, float y) const;
        static Circle create(float x1, float y1, float x2, float y2, float x3, float y3);
    };

    struct Edge {
        int v0, v1;
    };

    struct Triangle {
        int v0, v1, v2;
        Circle circle;
        Triangle();
        Triangle(int v0, int v1, int v2);
        static bool contains(const glm::vec2& pt, const glm::vec2& t0, const glm::vec2& t1, const glm::vec2& t2);
        static bool contains(const glm::vec3& pt, const glm::vec3& dir, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2);

    };

    //model
    std::vector<glm::vec2> points;
    std::vector<Edge> edges;
    std::vector<Triangle> triangles;

    //view
    std::vector<CircleMesh> pointsMesh;
    std::vector<CircleMesh> circlesMesh;
    std::vector<LineMesh> edgesMesh;

    //Builder
    int selectedIndex = -1;
    void addPoint(float x, float y);
    void selectPoint(float x, float y);
    void movePoint(float x, float y);
    void clearSelection();
    void triangulate();

    Triangle createTriangle(int v0, int v1, int v2);
    static CircleMesh createCircle(const Triangle& t);
};

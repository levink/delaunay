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
        Circle(glm::vec2 a, glm::vec2 b, glm::vec2 c);
        bool contains(float x, float y) const;
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
    CircleMesh selectedCircle;
    const CircleMesh* getSelectedCircle();

    //Builder
    int selectedPoint = -1;
    int selectedTriangle = -1;
    void addPoint(const glm::vec2& point);
    void movePoint(const glm::vec2& cursor);
    void selectPoint(const glm::vec2& cursor);
    void selectTriangle(const glm::vec2& cursor);
    void clearSelection();

    void triangulate(const glm::vec2& point);
    int findTriangle(const glm::vec2& point);
    Triangle createTriangle(int v0, int v1, int v2);

    void updateView();
    LineMesh createLineMesh(const Edge& edge);
    CircleMesh createPointMesh(const glm::vec2& point);
    CircleMesh createCircleMesh(const Triangle& triangle);
};

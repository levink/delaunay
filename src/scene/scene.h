#pragma once
#include "src/pch.h"
#include "../model/line.h"
#include "../model/circle.h"

struct Circle {
    glm::vec2 center;
    float radius;
    Circle();
    Circle(float x, float y, float radius);
    Circle(glm::vec2 a, glm::vec2 b, glm::vec2 c);
};

struct EdgeIndex {
    unsigned int v0, v1;
    EdgeIndex();
    EdgeIndex(unsigned int v0, unsigned int v1);
};

struct Triangle {
    glm::vec2 point0, point1, point2;
    unsigned int index0, index1, index2;
    unsigned int adjacent[3] = {0};

    bool contains(unsigned int index) const {
        return
                index0 == index ||
                index1 == index ||
                index2 == index;
    }
    bool contains(const glm::vec2& point) const {
        const float eps = 0.00000001f;
        auto pt = glm::vec3(point, 0);
        auto dir = glm::vec3(0, 0, 1);
        auto t0 = glm::vec3(point0, 0);
        auto t1 = glm::vec3(point1, 0);
        auto t2 = glm::vec3(point2, 0);

        if (glm::dot(glm::cross(t1 - t0, pt - t0), dir) < -eps) return false;
        if (glm::dot(glm::cross(pt - t0, t2 - t0), dir) < -eps) return false;
        if (glm::dot(glm::cross(t1 - pt, t2 - pt), dir) < -eps) return false;

        return true;
    }
    bool isAdjacentWith(const Triangle& triangle) const {

        int match = 0;
        if (triangle.contains(index0)) match++;
        if (triangle.contains(index1)) match++;
        if (triangle.contains(index2)) match++;

        return match >= 2;
    }
    void replaceAdjacent(unsigned int old_value, unsigned int new_value) {
        if (old_value == new_value){
            return;
        }

        for(auto& adj : adjacent) {
            if (adj == old_value) {
                adj = new_value;
                return;
            }
        }
    }
};

struct Scene {

    //model
    std::vector<glm::vec2> points;
    std::vector<Triangle> triangles;

    //view
    std::vector<CircleMesh> pointsMesh;
    std::vector<LineMesh> trianglesMesh;
    const CircleMesh* getSelectedCircle();

    glm::vec2 dragDrop;
    int selectedPoint = -1;
    int selectedTriangle = -1;

    void addPoint(const glm::vec2& cursor);
    void movePoint(const glm::vec2& cursor);
    void deletePoint(const glm::vec2& cursor);
    void selectPoint(const glm::vec2& cursor);
    void clearSelection();

    void initView(const glm::vec2& viewSize);
    void updateView();
    CircleMesh createPointMesh(const glm::vec2& point);
    CircleMesh createCircleMesh(const Circle& circle);
    LineMesh createLineMesh(const glm::vec2& start, const glm::vec2& end);

    float scale;
    glm::vec2 offset;
    void normalizePoints();
    void restorePoints();
    void addSuperTriangle();
    void removeSuperTriangle();
    void triangulate();
    void triangulatePoint(unsigned int pointIndex);
    int findTriangle(const glm::vec2& point);
};

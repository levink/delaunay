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
    bool contains(const glm::vec2& point) const {
        auto delta = point - center;
        return delta.x * delta.x + delta.y * delta.y <= radius * radius;
    }
};

struct EdgeIndex {
    unsigned int v0, v1;
    EdgeIndex();
    EdgeIndex(unsigned int v0, unsigned int v1);
};

struct Point {
    int index = -1;
    glm::vec2 position;
    Point() = default;
    Point(int index, float x, float y) : index(index), position(x,y) { }
    Point(int index, const glm::vec2& position) : index(index), position(position) { }
};


struct Triangle {
    int index = -1;
    Point point[3];
    int adjacent[3] = {
            -1, //edge for point0 - point1
            -1, //edge for point1 - point2
            -1  //edge for point2 - point0
    };
    Triangle() = default;
    Triangle(int index, Point p0, Point p1, Point p2) : index(index) {
        point[0] = p0;
        point[1] = p1;
        point[2] = p2;
    }

    bool has(int pointIndex) const {
        return
            point[0].index == pointIndex ||
            point[1].index == pointIndex ||
            point[2].index == pointIndex;
    }
    bool has(const Point& p) const {
        auto pointIndex = p.index;
        return
            point[0].index == pointIndex ||
            point[1].index == pointIndex ||
            point[2].index == pointIndex;
    }
    bool has(const Point& p1, const Point& p2) const {
        return has(p1.index) && has(p2.index);
    }
    bool contains(const glm::vec2& p) const {
        const float eps = 0.00000001f;
        auto pt = glm::vec3(p, 0);
        auto dir = glm::vec3(0, 0, 1);
        auto t0 = glm::vec3(point[0].position, 0);
        auto t1 = glm::vec3(point[1].position, 0);
        auto t2 = glm::vec3(point[2].position, 0);

        if (glm::dot(glm::cross(t1 - t0, pt - t0), dir) < -eps) return false;
        if (glm::dot(glm::cross(pt - t0, t2 - t0), dir) < -eps) return false;
        if (glm::dot(glm::cross(t1 - pt, t2 - pt), dir) < -eps) return false;

        return true;
    }
    bool isAdjacentWith(const Triangle& triangle) const {

        int match = 0;
        if (triangle.has(point[0])) match++;
        if (triangle.has(point[1])) match++;
        if (triangle.has(point[2])) match++;

        return match >= 2;
    }
    void replaceAdjacent(int old_value, int new_value) {
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
    int getOpposite(int pointIndex) {
        if (pointIndex == point[0].index) return adjacent[1];
        if (pointIndex == point[1].index) return adjacent[2];
        if (pointIndex == point[2].index) return adjacent[0];
        return -1;
    }
    int getAdjacent(int pointIndex0, int pointIndex1) {

        //todo this;
        return -1;
        if (pointIndex0 == point[0].index && pointIndex1 == point[1].index) return adjacent[0];
        if (pointIndex0 == point[1].index && pointIndex1 == point[2].index) return adjacent[1];
        if (pointIndex0 == point[2].index && pointIndex1 == point[0].index) return adjacent[2];

//        if (pointIndex1 == index[0] && pointIndex0 == index[1]) return adjacent[0];
//        if (pointIndex1 == index[1] && pointIndex0 == index[2]) return adjacent[1];
//        if (pointIndex1 == index[2] && pointIndex0 == index[0]) return adjacent[2];

        return -1;
    }
    bool link(const Triangle& triangle) {
        auto& p0 = point[0];
        auto& p1 = point[1];
        auto& p2 = point[2];

        if (triangle.has(p0, p1)) {
            adjacent[0] = triangle.index;
            return true;
        }
        else if (triangle.has(p1, p2)) {
            adjacent[1] = triangle.index;
            return true;
        }
        else if (triangle.has(p2, p0)) {
            adjacent[2] = triangle.index;
            return true;
        }

        return false;
    }
    Circle getCircle() const {
        return {
            point[0].position,
            point[1].position,
            point[2].position
        };
    }
};

struct Scene {

    //model
    std::vector<Point> points;
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
    void addPointToTriangulation(int pointIndex);
    int findTriangle(const glm::vec2& point);
    bool hasDelaunayConstraint(int triangleIndex) const {
        auto& triangle = triangles[triangleIndex];
        auto circle = triangle.getCircle();

        return false;
//        for(auto i = 0; i < points.size(); i++) {
//            if (triangle.has(i)) {
//                continue;
//            }
//
//            auto& point = points[i];
//            if (circle.contains(point)) {
//                return false;
//            }
//        }
//        return true;
    }
};

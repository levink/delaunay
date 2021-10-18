#pragma once
#include "src/pch.h"
#include "../model/line.h"
#include "../model/circle.h"

struct Util {
    template<typename T>
    static void shift(T values[3]){
        T first = values[0];
        for(auto i = 0; i < 2; i++) {
            values[i] = values[i+1];
        }
        values[2] = first;
    }
};

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

struct SplitPair {
    int forSplit = -1;
    int forCheck = -1;
};

struct SwapResult {
    bool success = false;
    SplitPair first;
    SplitPair second;
};

struct Edge {
    int v0, v1;
    Edge() : v0(-1), v1(-1) { }
    Edge(int v0, int v1) : v0(std::min(v0, v1)), v1(std::max(v0, v1)) {}
};

struct Point {
    int index = -1;
    float position[2] = {0};
    Point() = default;
    Point(int index, float x, float y) : index(index) {
        position[0] = x;
        position[1] = y;
    }
    Point(int index, const glm::vec2& pos) : index(index) {
        position[0] = pos.x;
        position[1] = pos.y;
    }
    glm::vec2 getPosition() const {
        return {position[0], position[1]};
    }
    void setPosition(const glm::vec2& value) {
        position[0] = value.x;
        position[1] = value.y;
    }
};

struct Hull {
    Point a,b,c,d;
    bool isConcave() const {
        //todo: this
    }
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
    bool linkedWith(const Triangle& t) const {
        for(auto i : adjacent){
            if (i == t.index) {
                return true;
            }
        }
        return false;
    }
    bool contains(const glm::vec2& p) const {
        const float eps = 0.00000001f;
        auto pt = glm::vec3(p, 0);
        auto dir = glm::vec3(0, 0, 1);
        auto t0 = glm::vec3(point[0].getPosition(), 0);
        auto t1 = glm::vec3(point[1].getPosition(), 0);
        auto t2 = glm::vec3(point[2].getPosition(), 0);

        if (glm::dot(glm::cross(t1 - t0, pt - t0), dir) < -eps) return false;
        if (glm::dot(glm::cross(pt - t0, t2 - t0), dir) < -eps) return false;
        if (glm::dot(glm::cross(t1 - pt, t2 - pt), dir) < -eps) return false;

        return true;
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
    int getOppositeTriangleIndex(int pointIndex) {
        if (pointIndex == point[0].index) return adjacent[1];
        if (pointIndex == point[1].index) return adjacent[2];
        if (pointIndex == point[2].index) return adjacent[0];
        return -1;
    }
    Hull getHull(const Triangle& adjacentTriangle) const {
        Hull result;
        if (adjacentTriangle.index == adjacent[0]) {
            result.a = point[2];
            result.b = point[0];
            result.d = point[1];
        }
        else if (adjacentTriangle.index == adjacent[1]) {
            result.a = point[0];
            result.b = point[1];
            result.d = point[2];
        }
        else if (adjacentTriangle.index == adjacent[2]) {
            result.a = point[1];
            result.b = point[2];
            result.d = point[0];
        } else {
            Log::warn("Something goes wrong!");
        }

        for(auto& p : adjacentTriangle.point) {
            bool isEdgePoint = p.index == result.b.index || p.index == result.d.index;
            if (!isEdgePoint) {
                result.c = p;
                break;
            }
        }

        return result;
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
            point[0].getPosition(),
            point[1].getPosition(),
            point[2].getPosition()
        };
    }
    void shift(){
        Util::shift(point);
        Util::shift(adjacent);
    }
    void setFirst(int pointIndex) {
        if (!has(pointIndex)) {
            return;
        }

        while(point[0].index != pointIndex){
            shift();
        }
    }
    void setLast(int pointIndex) {
        if (!has(pointIndex)) {
            return;
        }

        while(point[2].index != pointIndex) {
            shift();
        }
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

    void initScene(const glm::vec2& viewSize);
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
    int findTriangle(float point[2]);

    bool isConcaveHull(const SplitPair& pair) const {

        auto& t0 = triangles[pair.forSplit];
        auto& t1 = triangles[pair.forCheck];
        if (!t0.linkedWith(t1) || !t1.linkedWith(t0)) {
            Log::warn("Triangles are not linked!");
            return false;
        }

        auto hull = t0.getHull(t1);


        return true;
        t0.setFirst(splitPoint.index);
        old2.setLast(old1.point[1].index);

    }
    bool hasDelaunayConstraint(int triangleIndex) const {
        if (triangleIndex == -1) {
            return true;
        }

        auto& triangle = triangles[triangleIndex];
        auto circle = triangle.getCircle();

        for(auto& p : points) {
            if (triangle.has(p)) {
                continue;
            }

            if (circle.contains(p.getPosition())) {
                return false;
            }
        }
           return true;
    }

    SwapResult swapEdge(const Point& splitPoint, const SplitPair& splitPair) {
        return swapEdge(splitPoint, splitPair.forSplit, splitPair.forCheck);
    }
    SwapResult swapEdge(const Point& splitPoint, int tIndex1, int tIndex2);
};

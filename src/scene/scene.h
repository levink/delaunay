#pragma once
#include "pch.h"
#include "model/line.h"
#include "model/circle.h"
#include "platform/log.h"

namespace delaunay {

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

    struct Edge {
        int v0, v1;
        bool containsPoint(int index) const {
            return v0 == index || v1 == index;
        }
    };

    struct Point {
        int index = -1;
        glm::vec2 position;
        Point() = default;
        Point(int index, float x, float y) {
            this->index = index;
            this->position.x = x;
            this->position.y = y;
        }
        const glm::vec2& getPosition() const {
            return position;
        }
        void setPosition(const glm::vec2& value) {
            position = value;
        }
    };

    struct Hull {
        Point a,b,c,d;
        bool isConvex() const {
            const glm::vec2 edge[4] = {
                b.getPosition() - a.getPosition(),
                c.getPosition() - b.getPosition(),
                d.getPosition() - c.getPosition(),
                a.getPosition() - d.getPosition()
            };

            float first = 0;
            for (int i = 0; i < 4; i++)
            {
                auto& e1 = edge[i];
                auto& e2 = edge[(i + 1) % 4];

                if (i == 0) {
                    first = e1.x * e2.y - e1.y * e2.x;
                    continue;
                }

                auto last = e1.x * e2.y - e1.y * e2.x;
                if (first < 0 && last > 0 || first > 0 && last < 0) {
                    return false;
                }
            }

            return true;
        }
    };

    struct Box {
        glm::vec2 min;
        glm::vec2 max;

        bool contains(const glm::vec2& point) const {
            return
                min.x <= point.x && point.x <= max.x &&
                min.y <= point.y && point.y <= max.y;
        }
    };

    struct Triangle {
        int index = -1;
        Point point[3];
        int adjacent[3] = {
                -1, //triangle index for edge with point0 - point1
                -1, //triangle index for edge with point1 - point2
                -1  //triangle index for edge with point2 - point0
        };
        Box box;
        Triangle() = default;
        Triangle(int index, Point p0, Point p1, Point p2) : index(index) {
            point[0] = p0;
            point[1] = p1;
            point[2] = p2;
            updateBox();
        }
        void updateBox() {

            const glm::vec2& p0 = point[0].getPosition();
            const glm::vec2& p1 = point[1].getPosition();
            const glm::vec2& p2 = point[2].getPosition();

            box.min = p0;
            box.min.x = std::min(box.min.x, p1.x);
            box.min.x = std::min(box.min.x, p2.x);
            box.min.y = std::min(box.min.y, p1.y);
            box.min.y = std::min(box.min.y, p2.y);

            box.max = p0;
            box.max.x = std::max(box.max.x, p1.x);
            box.max.x = std::max(box.max.x, p2.x);
            box.max.y = std::max(box.max.y, p1.y);
            box.max.y = std::max(box.max.y, p2.y);
        }
        bool hasError() const {
            if (point[0].index == point[1].index) return true;
            if (point[1].index == point[2].index) return true;
            if (point[2].index == point[0].index) return true;
            return false;
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

            if (!box.contains(p)) {
                Log::out("!box.contains(p)");
                return false;
            }

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
        int getOpposite(int pointIndex) {
            if (pointIndex == point[0].index) return adjacent[1];
            if (pointIndex == point[1].index) return adjacent[2];
            if (pointIndex == point[2].index) return adjacent[0];

            std::string msg = "Triangle does not have point with index=" + std::to_string(pointIndex);
            Log::warn(msg);
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

            Log::warn("Can not link");
            return false;
        }
        Circle getCircle() const {
            return {
                point[0].getPosition(),
                point[1].getPosition(),
                point[2].getPosition()
            };
        }
        void setFirst(int pointIndex) {
            if (!has(pointIndex)) {
                return;
            }

            while(point[0].index != pointIndex){
                Util::shift(point);
                Util::shift(adjacent);
            }
            updateBox();
        }
        Edge getCommonEdge(const Triangle& other) {
            if (other.index == adjacent[0]) return Edge{point[0].index, point[1].index};
            if (other.index == adjacent[1]) return Edge{point[1].index, point[2].index};
            if (other.index == adjacent[2]) return Edge{point[2].index, point[0].index};
            throw std::runtime_error("something goes wrong");
        }
        int getOppositePoint(const Edge& edge) {
            int matches = 0;
            if (edge.containsPoint(point[0].index)) matches++;
            if (edge.containsPoint(point[1].index)) matches++;
            if (edge.containsPoint(point[2].index)) matches++;

            if (matches != 2) {
                auto msg = "[Triangle::getOppositePoint] Bad edge check";
                Log::warn(msg);
                throw std::runtime_error(msg);
            }

            if (!edge.containsPoint(point[0].index))  return point[0].index;
            if (!edge.containsPoint(point[1].index))  return point[1].index;
            if (!edge.containsPoint(point[2].index))  return point[2].index;

            auto msg = "[Triangle::getOppositePoint] Bad edge result";
            Log::warn(msg);
            throw std::runtime_error(msg);
        }
        void update(const Point& p) {
            if (p.index == point[0].index) {
                point[0].setPosition(p.getPosition());
            }
            else if (p.index == point[1].index) {
                point[1].setPosition(p.getPosition());
            }
            else if (p.index == point[2].index) {
                point[2].setPosition(p.getPosition());
            }
            updateBox();
        }
    };

    struct SwapResult {
        bool success = false;
        int first = -1;
        int second = -1;
    };

    struct SceneModel {
        std::vector<Point> points;
        std::vector<Triangle> triangles;
        glm::vec2 dragOffset;
        int selectedPointIndex = -1;        

        void init(float widthPx, float heightPx);
        void addPoint(float x, float y);
        void movePoint(size_t index, const glm::vec2& position);
        void triangulate();
        bool super(const Point& point) const;
        bool super(const Triangle& tr) const;

    private:
        Triangle* findTriangle(const glm::vec2& point);
        std::stack<int> split(Triangle triangleForSplit, Point point);
        SwapResult swapEdge(const Point& splitPoint, Triangle& t1, Triangle& t2);
        void swapEdges(std::stack<int>& trianglesForCheck, const Point& point);
        void checkError(const Triangle& triangle);
    };

    struct SelectedPoint {
        bool active;
        CircleMesh mesh;
    };

    struct SceneView {
        std::vector<CircleMesh> pointsMesh;
        std::vector<LineMesh> trianglesMesh;
        SelectedPoint selectedPoint;

        void init(const SceneModel& model);
        void addPoint(const Point& point);
        void updateSelected(size_t index, const glm::vec2& position);
        void updateTriangles(const SceneModel& model);

    };

    struct Scene {
        SceneModel model;
        SceneView view;
        void init(const glm::vec2& viewSize);
        void addPoint(const glm::vec2& cursor);
        void movePoint(const glm::vec2& cursor);
        void deletePoint(const glm::vec2& cursor);
        void selectPoint(const glm::vec2& cursor);
        void clearSelection();
    private:
        int nearestPoint(const glm::vec2& cursor, float& distance);
    };
};
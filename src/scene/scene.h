#pragma once

#include "pch.h"
#include "model/line.h"
#include "model/circle.h"
#include "platform/log.h"
#include "scene/drawBatch.h"
#include <set>

namespace delaunay {

    struct Util {
        template<typename T>
        static void shiftLeft(T values[3]){
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
        uint32_t v0, v1;
        bool containsPoint(int pointId) const {
            return v0 == pointId || v1 == pointId;
        }
    };

    struct Point {
        uint32_t id = 0;
        glm::vec2 position;
        Point() = default;
        Point(uint32_t id, float x, float y) :
            id(id),
            position(x, y) { }
    };

    struct Hull {
        glm::vec2 a,b,c,d;
        bool isConvex() const {
            const glm::vec2 edge[4] = {
                b - a,
                c - b,
                d - c,
                a - d
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
        bool contains(float x, float y) const {
            return
                min.x <= x && x <= max.x &&
                min.y <= y && y <= max.y;
        }
    };

    struct Triangle {
        uint32_t id = -1;
        Point* point[3];
        Triangle* adjacent[3] = {
                nullptr,  //triangle for edge with point0 - point1
                nullptr,  //triangle for edge with point1 - point2
                nullptr   //triangle for edge with point2 - point0
        };
        Box box;
        Triangle() = default;
        Triangle(uint32_t id, Point* p0, Point* p1, Point* p2) :
            id(id) {
            point[0] = p0;
            point[1] = p1;
            point[2] = p2;
            updateBox();
        }
        void setPoints(Point* p0, Point* p1, Point* p2) {
            point[0] = p0;
            point[1] = p1;
            point[2] = p2;
            updateBox();
        }
        void setAdjacent(Triangle* a0, Triangle* a1, Triangle* a2) {
            adjacent[0] = a0;
            adjacent[1] = a1;
            adjacent[2] = a2;

            if (a0) {
                a0->link(this);
            }
            if (a1) {
                a1->link(this);
            }
            if (a2) {
                a2->link(this);
            }
        }
        void shiftPointFirst(int firstPointId) {
            if (!hasPoint(firstPointId)) {
                return;
            }

            while (point[0]->id != firstPointId) {
                Util::shiftLeft(point);
                Util::shiftLeft(adjacent);
            }
        }
        void updateBox() {

            const glm::vec2& p0 = point[0]->position;
            const glm::vec2& p1 = point[1]->position;
            const glm::vec2& p2 = point[2]->position;

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

        bool hasPoint(int pointId) const {
            return
                point[0]->id == pointId ||
                point[1]->id == pointId ||
                point[2]->id == pointId;
        }
        bool hasPoints(const Point& p1, const Point& p2) const {
            return 
                hasPoint(p1.id) && 
                hasPoint(p2.id);
        }
        bool link(Triangle* triangle) {
            auto& p0 = *point[0];
            auto& p1 = *point[1];
            auto& p2 = *point[2];

            if (triangle->hasPoints(p0, p1)) {
                adjacent[0] = triangle;
                return true;
            }
            else if (triangle->hasPoints(p1, p2)) {
                adjacent[1] = triangle;
                return true;
            }
            else if (triangle->hasPoints(p2, p0)) {
                adjacent[2] = triangle;
                return true;
            }

            Log::warn("Can not link");
            return false;
        }
        bool linkedWith(const Triangle* triangle) const {
            const bool linked = 
                adjacent[0] == triangle ||
                adjacent[1] == triangle ||
                adjacent[2] == triangle;
            return linked;
        }
        bool contains(float x, float y) const {

            if (!box.contains(x, y)) {
                return false;
            }

            const float eps = 0.000001f;
            auto pt = glm::vec3(x, y, 0);
            auto dir = glm::vec3(0, 0, 1);
            auto t0 = glm::vec3(point[0]->position, 0);
            auto t1 = glm::vec3(point[1]->position, 0);
            auto t2 = glm::vec3(point[2]->position, 0);

            if (glm::dot(glm::cross(t1 - t0, pt - t0), dir) < -eps) return false;
            if (glm::dot(glm::cross(pt - t0, t2 - t0), dir) < -eps) return false;
            if (glm::dot(glm::cross(t1 - pt, t2 - pt), dir) < -eps) return false;

            return true;
        }
        int getOppositePoint(const Edge& edge) const {
            int matches = 0;
            if (edge.containsPoint(point[0]->id)) matches++;
            if (edge.containsPoint(point[1]->id)) matches++;
            if (edge.containsPoint(point[2]->id)) matches++;

            if (matches != 2) {
                auto msg = "[Triangle::getOppositePoint] Bad edge check";
                Log::warn(msg);
                throw std::runtime_error(msg);
            }

            if (!edge.containsPoint(point[0]->id))  return point[0]->id;
            if (!edge.containsPoint(point[1]->id))  return point[1]->id;
            if (!edge.containsPoint(point[2]->id))  return point[2]->id;

            auto msg = "[Triangle::getOppositePoint] Bad edge result";
            Log::warn(msg);
            throw std::runtime_error(msg);
        }
        Triangle* getOppositeTriangle(int pointId) const {
            if (pointId == point[0]->id) return adjacent[1];
            if (pointId == point[1]->id) return adjacent[2];
            if (pointId == point[2]->id) return adjacent[0];

            std::string msg = "Triangle does not have pointId=" + std::to_string(pointId);
            Log::warn(msg);
            return nullptr;
        }
        Hull getHull(const Triangle* other) const {
            
            glm::vec2 a, b, c, d;
            int e1 = -1;
            int e2 = -1;
            if (other == adjacent[0]) {
                a = point[2]->position;
                b = point[0]->position;
                d = point[1]->position;
                e1 = point[0]->id;
                e2 = point[1]->id;
            }
            else if (other == adjacent[1]) {
                a = point[0]->position;
                b = point[1]->position;
                d = point[2]->position;
                e1 = point[1]->id;
                e2 = point[2]->id;
            }
            else if (other == adjacent[2]) {
                a = point[1]->position;
                b = point[2]->position;
                d = point[0]->position;
                e1 = point[2]->id;
                e2 = point[0]->id;
            } else {
                Log::warn("Something goes wrong!");
            }

            for(auto& adjacentPoint : other->point) {
                bool isCommonEdgePoint =
                    (adjacentPoint->id == e1) ||
                    (adjacentPoint->id == e2);
                if (isCommonEdgePoint) {
                    continue;
                }
                c = adjacentPoint->position;
            }

            return Hull{ a, b, c, d };
        }
        Edge getCommonEdge(const Triangle* other) const {
            if (other == adjacent[0]) return Edge{point[0]->id, point[1]->id};
            if (other == adjacent[1]) return Edge{point[1]->id, point[2]->id};
            if (other == adjacent[2]) return Edge{point[2]->id, point[0]->id};
            throw std::runtime_error("something goes wrong");
        }
        Circle getCircle() const {
            return Circle{
                point[0]->position,
                point[1]->position,
                point[2]->position
            };
        }
    };

   
    struct SplitResult {
        bool success = false;
        const Triangle* a = nullptr;
        const Triangle* b = nullptr;
        const Triangle* c = nullptr;
    };

    struct SceneModel {
        struct Observer {
            virtual void onUpdate(const SceneModel& model) = 0;
        };
        
        std::vector<Point*> points;         //todo: clear memory (points & triangles)
        std::vector<Triangle*> triangles;   //todo: clear memory (points & triangles)
        uint32_t errors = 0;
        std::vector<const Point*> changedPoints;
        std::set<const Triangle*> changedTriangles;   
    private:
        std::stack<int> trianglesForCheck;
        
    public: 
        SceneModel() = default;
        ~SceneModel();
        void addPoint(float x, float y);
        void movePoint(size_t id, const glm::vec2& position);
        void updateView(Observer& observer);
        int nearestPointIndex(const glm::vec2& cursor, float& distance);
        void printResult();
    private:
        void increaseError();
        Triangle* findTriangle(float x, float y);
        SplitResult splitTriangle(Triangle* triangleForSplit, Point* point);
        bool swapEdge(Triangle* t1, Triangle* t2);
        void checkDelaunayConstraint(std::stack<int>& trianglesForCheck, const Point& point);
        bool hasDelaunayConstraint(const Triangle* t1, const Triangle* t2);
        //void linkTriangleByIndex(int adjacentIndex, const Triangle& target); //todo: no need?
        void orderTriangles();
    };

    struct SceneView : public SceneModel::Observer {
        std::vector<CircleMesh> pointMeshes;
        std::vector<LineMesh> triangleMeshes;
        void onUpdate(const SceneModel& model) override;
    };

    struct SelectedPoint {
        int index = -1;
        CircleMesh mesh;
    };

    struct Scene {
        DrawBatch background;
        SceneModel model;
        SceneView view;
        glm::vec2 dragOffset;
        SelectedPoint selectedPoint;

        void init(const glm::vec2& viewSize);
        void destroy();
        void addPoint(const glm::vec2& cursor);
        void movePoint(const glm::vec2& cursor);
        void selectPoint(const glm::vec2& cursor);
        void clearSelection();
    };
};
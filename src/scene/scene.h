#pragma once

#include "pch.h"
#include "model/line.h"
#include "model/circle.h"
#include "platform/log.h"
#include "scene/drawBatch.h"
#include <set>

namespace delaunay {

    struct Circle {
        glm::vec2 center;
        float radius;
        Circle(glm::vec2 a, glm::vec2 b, glm::vec2 c);
        bool contains(const glm::vec2& point) const;
    };

    struct Box {
        glm::vec2 min;
        glm::vec2 max;
        bool contains(float x, float y) const;
    };

    struct Edge {
        uint32_t p0, p1;
        bool hasPoint(int pointId) const;
    };

    struct Hull {
        glm::vec2 a, b, c, d;
        bool isConvex() const;
    };

    struct Point {
        uint32_t id = 0;
        glm::vec2 position;
        Point(uint32_t id, float x, float y);
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

        Triangle(uint32_t id, Point* p0, Point* p1, Point* p2);
        void setPoints(Point* p0, Point* p1, Point* p2);
        void setAdjacent(Triangle* a0, Triangle* a1, Triangle* a2);
        void shiftPointFirst(int firstPointId);
        void updateBox();
        bool hasPoint(int pointId) const;
        bool hasPoints(const Point& p1, const Point& p2) const;
        bool link(Triangle* triangle);
        bool linkedWith(const Triangle* triangle) const;
        bool contains(float x, float y) const;
        Point* getOppositePoint(const Edge& edge) const;
        Triangle* getOppositeTriangle(int pointId) const;
        Hull getHull(const Triangle* other) const;
        Edge getCommonEdge(const Triangle* other) const;
        Circle getCircle() const;
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
        std::vector<Point*> points;  
        std::vector<Triangle*> triangles;
        std::vector<const Point*> changedPoints;
        std::set<const Triangle*> changedTriangles;
    private:
        uint32_t errors = 0;
        std::stack<int> trianglesForCheck;

    public: 
        SceneModel() = default;
        ~SceneModel();
        bool hasErrors() const;
        void addPoint(float x, float y);
        void movePoint(size_t id, const glm::vec2& position);
        void updateView(Observer& observer);
    private:
        void increaseError();
        Triangle* findTriangle(float x, float y);
        SplitResult splitTriangle(Triangle* triangleForSplit, Point* point);
        bool swapEdge(Triangle* t1, Triangle* t2);
        void checkDelaunayConstraint(std::stack<int>& trianglesForCheck, const Point& point);
        bool hasDelaunayConstraint(const Triangle* t1, const Triangle* t2);
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
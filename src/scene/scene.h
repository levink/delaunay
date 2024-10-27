#pragma once

#include "pch.h"
#include "model/line.h"
#include "model/circle.h"
#include "platform/log.h"
#include "scene/drawBatch.h"
#include <unordered_set>

namespace delaunay {

    struct DCircle {
        glm::vec2 center;
        float radius = 0.f;
        DCircle() = default;
        DCircle(glm::vec2 a, glm::vec2 b, glm::vec2 c);
        bool contains(const glm::vec2& point) const;
    };

    struct DBox {
        glm::vec2 min;
        glm::vec2 max;
        bool contains(float x, float y) const;
    };

    struct DEdge {
        uint32_t p0, p1; //Guaranteed that p0 <= p1 
        DEdge(uint32_t p0, uint32_t p1);
        bool hasPoint(uint32_t pointId) const;
        uint64_t key() const;
    };

    struct DPoint {
        uint32_t id = 0;
        glm::vec2 position;
        DPoint(uint32_t id, float x, float y);
    };

    struct DTriangle {
        uint32_t id = -1;
        DPoint* points[3];
        DTriangle* adjacent[3] = {
                nullptr,  //triangle for edge with point0 - point1
                nullptr,  //triangle for edge with point1 - point2
                nullptr   //triangle for edge with point2 - point0
        };
        DBox box;
        DCircle circle;

        DTriangle(uint32_t id, DPoint* p0, DPoint* p1, DPoint* p2);
        void setPoints(DPoint* p0, DPoint* p1, DPoint* p2);
        bool setAdjacent(DTriangle* a0, DTriangle* a1, DTriangle* a2);
        void update();
        bool link(DTriangle* triangle);
        bool linkedWith(const DTriangle* triangle) const;
        bool hasPointId(uint32_t pointId) const;
        bool hasPoint(const DPoint* point) const;
        bool hasPoints(const DPoint* p1, const DPoint* p2) const;
        bool contains(float x, float y, uint8_t& hitCode) const;
        DPoint* getOppositePoint(const DEdge& edge) const;
        DTriangle* getAdjacentTriangle(const DPoint* point) const;
        DEdge getCommonEdge(const DTriangle* other) const;
        DEdge getOppositeEdge(const DPoint* point) const;
        DEdge getEdge(int edgeIndex) const;
    };

    struct SplitResult {
        bool ok = false;
        const DTriangle* tr[4] = {
            nullptr,
            nullptr,
            nullptr,
            nullptr
        };
    };

    struct Triangulation {

        struct Observer {
            virtual void getUpdates(const Triangulation& model) = 0;
        };

        DBox workBox;
        std::vector<DPoint*> points;  
        std::vector<DTriangle*> triangles;
        std::unordered_set<const DPoint*> changedPoints;
        std::unordered_set<const DTriangle*> changedTriangles;
    private:
        size_t superOffset = 0;
        uint32_t errors = 0;
        std::stack<uint32_t> trianglesForCheck;
        std::unordered_set<uint64_t> markedEdges;

    public: 
        Triangulation() = default;
        ~Triangulation();
        void setWorkBox(float startX, float startY, float endX, float endY);
        void addPoint(float x, float y);
        void movePoint(size_t id, const glm::vec2& position);
        void updateView(Observer& observer);
        void rebuild();
        bool isSuper(const DPoint* point) const;
        bool isSuper(const DTriangle* tr) const;
        bool isDelaynayConstrained(size_t tIndex) const;
        bool hasErrors() const;
    private:
        void increaseError();
        void addPoint(DPoint* point);
        DTriangle* findTriangle(float x, float y, uint8_t& hitCode);
        SplitResult splitTriangle(DTriangle* triangle, uint8_t hitCode, DPoint* point);
        SplitResult splitOneByInternalPoint(DTriangle* triangle, DPoint* point);
        SplitResult splitOneByEdge(DTriangle* triangle, int edgeNum, DPoint* point);
        SplitResult splitTwoByEdge(DTriangle* triangle, DTriangle* adjacent, DPoint* point);
        void checkDelaunayLocally(DTriangle* target, DTriangle* adjacent);
        bool swapEdge(DTriangle* t1, DTriangle* t2);
        void markEdge(DTriangle* target, uint8_t edgeNumber);
    };

    struct SceneView : public Triangulation::Observer {
        std::vector<CircleMesh> pointMeshes;
        std::vector<LineMesh> triangleMeshes;
        void getUpdates(const Triangulation& model) override;
    };

    struct SelectedPoint {
        int index = -1;
        CircleMesh mesh;
    };

    struct Scene {
        DrawBatch background;
        Triangulation model;
        SceneView view;
        glm::vec2 dragOffset;
        SelectedPoint selectedPoint;

        void init(const glm::vec2& viewSize);
        void destroy();
        void addPoint(const glm::vec2& cursor);
        void movePoint(const glm::vec2& cursor);
        void selectPoint(const glm::vec2& cursor);
        void recover();
        void rebuild();
    };
};

#pragma once
#include <glm/vec2.hpp>
#include <vector>
#include <array>
#include <stack>
#include <functional>
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
        uint32_t p0 = 0;
        uint32_t p1 = 0;
        DEdge(uint32_t p0, uint32_t p1); //Guaranteed that this->p0 <= this->p1 
        bool hasPoint(int pointId) const;
        bool operator==(const DEdge& other) const;
    };

    struct DHull {
        glm::vec2 a, b, c, d;
        bool isConvex() const;
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
        bool hasPointId(int pointId) const;
        bool hasPoint(const DPoint* point) const;
        bool hasPoints(const DPoint* p1, const DPoint* p2) const;
        bool contains(float x, float y, uint8_t& hitCode) const;
        DPoint* getOppositePoint(const DEdge& edge) const;
        DPoint* getPoint(uint32_t pointId) const;
        DTriangle* getAdjacentTriangle(const DPoint* point) const;
        DEdge getCommonEdge(const DTriangle* other) const;
        DEdge getOppositeEdge(const DPoint* point) const;
        DEdge getEdge(int edgeIndex) const;
        DHull getHull(const DTriangle* other) const;
    };

    struct SplitResult {
        bool ok = false;
        std::array<const DTriangle*, 4> changedTriangles;
        std::vector<DEdge> edgesForCheck;
    };

    struct DEdgeHash {
        size_t operator()(const DEdge& edge) const;
    };

    struct UnuqEdgeStack {
        std::stack<DEdge> itemStack;
        std::unordered_set<DEdge, DEdgeHash> itemSet;

        void push(const DEdge& edge);
        DEdge pop();
        bool empty() const;
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
        UnuqEdgeStack edgeStack;

    public:
        Triangulation() = default;
        ~Triangulation();
        void setWorkBox(float startX, float startY, float endX, float endY);
        void addPoint(float x, float y);
        void movePoint(size_t pointIndex, const glm::vec2& position);
        void updateView(Observer& observer);
        void rebuild();
        bool isSuper(const DPoint* point) const;
        bool isSuper(const DTriangle* tr) const;
        bool isDelaynayConstrained(size_t tIndex) const;
        bool hasErrors() const;
        void swapEdgeManually(const DEdge& edge);
    private:
        void increaseError();
        void addPoint(DPoint* point);
        void createSuperStructure();
        DTriangle* findTriangle(float x, float y, uint8_t& hitCode);
        SplitResult splitTriangle(DTriangle* triangle, uint8_t hitCode, DPoint* point);
        SplitResult splitOneByInternalPoint(DTriangle* triangle, DPoint* point);
        SplitResult splitOneByEdge(DTriangle* triangle, int edgeNum, DPoint* point);
        SplitResult splitTwoByEdge(DTriangle* triangle, DTriangle* adjacent, DPoint* point);
        void checkAndSwapEdges();
        bool swapEdge(DTriangle* t1, DTriangle* t2);
        bool legalEdge(const DEdge& edge, DTriangle* t1, DTriangle* t2) const;
    };
}

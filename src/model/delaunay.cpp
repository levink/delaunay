#include "delaunay.h"

namespace math {
    constexpr float SAME_EPS = 0.001f;
    static bool same(float a, float b) {
        auto delta = std::abs(a - b);
        return delta < SAME_EPS;
    }
    static bool same(const glm::vec2& a, const glm::vec2& b) {
        auto dx = std::abs(a.x - b.x);
        auto dy = std::abs(a.y - b.y);
        auto delta = dx + dy;
        return delta < SAME_EPS;
    }
    static float trim(float value, float min, float max) {
        if (value < min) {
            value = min;
        }

        if (value > max) {
            value = max;
        }

        return value;
    }
}

namespace delaunay {

    namespace util {
        static bool hasErrors(const DTriangle* item) {
            const auto& p = item->points;
            bool samePoints =
                (p[0]->id == p[1]->id) ||
                (p[1]->id == p[2]->id) ||
                (p[2]->id == p[0]->id);
            if (samePoints) {
                return true;
            }

            auto dirAB = p[1]->position - p[0]->position;
            auto dirAC = p[2]->position - p[0]->position;
            auto dot = abs(glm::dot(dirAB, dirAC));
            bool oneLine = math::same(1, dot);
            if (oneLine) {
                return true;
            }

            const auto& a = item->adjacent;
            bool sameAdjacents =
                (a[0] == a[1] && a[0]) ||
                (a[0] == a[2] && a[0]) ||
                (a[1] == a[2] && a[1]);
            if (sameAdjacents) {
                return true;
            }

            return false;
        }
        static int nearestIndex(const std::vector<DPoint*>& points, const glm::vec2& targetFrom, float& resultDistance) {
            if (points.empty()) {
                return -1;
            }

            auto index = 0;
            auto minDistance = glm::distance(points[0]->position, targetFrom);
            for (int i = 1; i < points.size(); i++) {
                auto& position = points[i]->position;
                float distance = glm::distance(position, targetFrom);
                if (distance < minDistance) {
                    minDistance = distance;
                    index = i;
                }
            }

            resultDistance = minDistance;
            return index;
        }
    }

    DCircle::DCircle(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
        float x1 = a.x; float y1 = a.y;
        float x2 = b.x; float y2 = b.y;
        float x3 = c.x; float y3 = c.y;

        float m1 = (x1 * x1 + y1 * y1);
        float m2 = (x2 * x2 + y2 * y2);
        float m3 = (x3 * x3 + y3 * y3);

        float A = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
        float B = m1 * (y3 - y2) + m2 * (y1 - y3) + m3 * (y2 - y1);
        float C = m1 * (x2 - x3) + m2 * (x3 - x1) + m3 * (x1 - x2);
        float D = m1 * (x3 * y2 - x2 * y3) + m2 * (x1 * y3 - x3 * y1) + m3 * (x2 * y1 - x1 * y2);

        center.x = -B / (2 * A);
        center.y = -C / (2 * A);
        radius = sqrt((B * B + C * C - 4 * A * D) / (4 * A * A));
    }
    bool DCircle::contains(const glm::vec2& point) const {
        auto delta = point - center;
        auto length = sqrt(delta.x * delta.x + delta.y * delta.y);
        return length < radius * 0.995f;
    }
    bool DBox::contains(float x, float y) const {
        return
            min.x <= x && x <= max.x &&
            min.y <= y && y <= max.y;
    }
    bool DHull::isConvex() const {
        const glm::vec2 edge[4] = {
            b - a,
            c - b,
            d - c,
            a - d
        };

        float first = 0;
        for (int i = 0; i < 4; i++)
        {
            const auto& e1 = edge[i];
            const auto& e2 = edge[(i + 1) % 4];

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

    DEdge::DEdge(uint32_t p0, uint32_t p1) {
        if (p0 > p1) {
            std::swap(p0, p1);
        }
        this->p0 = p0;
        this->p1 = p1;
    }
    bool DEdge::hasPoint(int pointId) const {
        return p0 == pointId || p1 == pointId;
    }
    bool DEdge::operator==(const DEdge& other) const {
        return
            this->hasPoint(other.p0) &&
            this->hasPoint(other.p1);
    }

    DPoint::DPoint(uint32_t id, float x, float y) :
        id(id),
        position(x, y) { }

    DTriangle::DTriangle(uint32_t id, DPoint* p0, DPoint* p1, DPoint* p2) :
        id(id) {
        points[0] = p0;
        points[1] = p1;
        points[2] = p2;
        update();
    }
    void DTriangle::setPoints(DPoint* p0, DPoint* p1, DPoint* p2) {
        points[0] = p0;
        points[1] = p1;
        points[2] = p2;
        update();
    }
    bool DTriangle::setAdjacent(DTriangle* a0, DTriangle* a1, DTriangle* a2) {
        adjacent[0] = a0;
        adjacent[1] = a1;
        adjacent[2] = a2;

        bool allIsNull =
            a0 == nullptr &&
            a1 == nullptr &&
            a2 == nullptr;
        if (allIsNull) {
            return true;
        }

        bool linked = true;
        if (a0 && a0 != this) {
            linked &= a0->link(this);
        }
        if (a1 && a1 != this) {
            linked &= a1->link(this);
        }
        if (a2 && a2 != this) {
            linked &= a2->link(this);
        }
        return linked;
    }
    void DTriangle::update() {

        //Update box
        const glm::vec2& p0 = points[0]->position;
        const glm::vec2& p1 = points[1]->position;
        const glm::vec2& p2 = points[2]->position;

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

        //Update circle
        circle = DCircle{
            points[0]->position,
            points[1]->position,
            points[2]->position
        };
    }
    bool DTriangle::link(DTriangle* triangle) {
        if (triangle->hasPoints(points[0], points[1])) {
            adjacent[0] = triangle;
            return true;
        }
        if (triangle->hasPoints(points[1], points[2])) {
            adjacent[1] = triangle;
            return true;
        }
        if (triangle->hasPoints(points[2], points[0])) {
            adjacent[2] = triangle;
            return true;
        }
        return false;
    }
    bool DTriangle::linkedWith(const DTriangle* triangle) const {
        if (adjacent[0] == triangle) {
            return triangle->hasPoints(points[0], points[1]);
        }
        if (adjacent[1] == triangle) {
            return triangle->hasPoints(points[1], points[2]);
        }
        if (adjacent[2] == triangle) {
            return triangle->hasPoints(points[2], points[0]);
        }
        return false;
    }
    bool DTriangle::hasPointId(int pointId) const {
        return
            points[0]->id == pointId ||
            points[1]->id == pointId ||
            points[2]->id == pointId;
    }
    bool DTriangle::hasPoint(const DPoint* point) const {
        return
            points[0] == point ||
            points[1] == point ||
            points[2] == point;
    }
    bool DTriangle::hasPoints(const DPoint* p1, const DPoint* p2) const {
        return hasPoint(p1) && hasPoint(p2);
    }
    bool DTriangle::contains(float x, float y, uint8_t& hitCode) const {
        hitCode = 0;

        if (!box.contains(x, y)) {
            return false;
        }

        constexpr float eps = 0.00001f;
        auto pt = glm::vec3(x, y, 0);
        auto dir = glm::vec3(0, 0, 1);
        auto t0 = glm::vec3(points[0]->position, 0);
        auto t1 = glm::vec3(points[1]->position, 0);
        auto t2 = glm::vec3(points[2]->position, 0);

        auto dot1 = glm::dot(glm::cross(t1 - t0, pt - t0), dir);
        auto dot2 = glm::dot(glm::cross(t2 - t1, pt - t1), dir);
        auto dot3 = glm::dot(glm::cross(t0 - t2, pt - t2), dir);

        bool insideCW = dot1 > 0 && dot2 > 0 && dot3 > 0;
        bool insideCCW = dot1 < 0 && dot2 < 0 && dot3 < 0;
        if (insideCW || insideCCW) {
            return true;
        }

        uint32_t side1 = 0;
        uint32_t side2 = 0;

        if (dot1 > 0) side1++;
        else if (dot1 < 0) side2++;

        if (dot2 > 0) side1++;
        else if (dot2 < 0) side2++;

        if (dot3 > 0) side1++;
        else if (dot3 < 0) side2++;

        if (side1 == 0 || side2 == 0) {
            //remember edge hits as bitmask
            if (std::abs(dot1) < eps) hitCode |= (1 << 0); //1 bit - hit edge0
            if (std::abs(dot2) < eps) hitCode |= (1 << 1); //2 bit - hit edge1
            if (std::abs(dot3) < eps) hitCode |= (1 << 2); //3 bit - hit edge2
        }
        return hitCode;
    }
    DPoint* DTriangle::getOppositePoint(const DEdge& edge) const {
        if (!edge.hasPoint(points[0]->id)) return points[0];
        if (!edge.hasPoint(points[1]->id)) return points[1];
        if (!edge.hasPoint(points[2]->id)) return points[2];

        return nullptr;
    }
    DPoint* DTriangle::getPoint(uint32_t pointId) const {
        if (points[0]->id == pointId) return points[0];
        if (points[1]->id == pointId) return points[1];
        if (points[2]->id == pointId) return points[2];

        return nullptr;
    }
    DTriangle* DTriangle::getAdjacentTriangle(const DPoint* point) const {
        if (points[0] == point) return adjacent[1];
        if (points[1] == point) return adjacent[2];
        if (points[2] == point) return adjacent[0];
        return nullptr;
    }
    DEdge DTriangle::getCommonEdge(const DTriangle* other) const {
        if (other == adjacent[0]) return DEdge{ points[0]->id, points[1]->id };
        if (other == adjacent[1]) return DEdge{ points[1]->id, points[2]->id };
        if (other == adjacent[2]) return DEdge{ points[2]->id, points[0]->id };

        return DEdge{ 0, 0 }; //something unexpected: triangles are not adjacent
    }
    DEdge DTriangle::getOppositeEdge(const DPoint* point) const {
        if (point == points[0]) return DEdge{ points[1]->id, points[2]->id };
        if (point == points[1]) return DEdge{ points[0]->id, points[2]->id };
        if (point == points[2]) return DEdge{ points[0]->id, points[1]->id };

        return DEdge{ 0,0 }; //bad case
    }
    DEdge DTriangle::getEdge(int edgeIndex) const {
        int e1 = edgeIndex % 3;
        int e2 = (edgeIndex + 1) % 3;
        return DEdge{ points[e1]->id, points[e2]->id };
    }
    DHull DTriangle::getHull(const DTriangle* other) const {

        DPoint* e0 = nullptr;
        DPoint* e1 = nullptr;
        if (other == adjacent[0]) {
            e0 = points[0];
            e1 = points[1];
        }
        else if (other == adjacent[1]) {
            e0 = points[1];
            e1 = points[2];
        }
        else if (other == adjacent[2]) {
            e0 = points[2];
            e1 = points[0];
        }
        else {
            //something unexpected: triangles are not adjacent
        }

        auto id0 = e0 ? e0->id : 0;
        auto id1 = e1 ? e1->id : 0;
        auto edge = DEdge{ id0, id1 };
        auto p0 = this->getOppositePoint(edge);
        auto p1 = other->getOppositePoint(edge);

        glm::vec2 a = p0 ? p0->position : glm::vec2(0);
        glm::vec2 b = e0 ? e0->position : glm::vec2(0);
        glm::vec2 c = p1 ? p1->position : glm::vec2(0);
        glm::vec2 d = e1 ? e1->position : glm::vec2(0);

        return DHull{ a, b, c, d };
    }

    size_t DEdgeHash::operator()(const DEdge& edge) const {
        return
            static_cast<uint64_t>(edge.p0) << 32 |
            static_cast<uint64_t>(edge.p1);
    }
    void UnuqEdgeStack::push(const DEdge& edge) {
        // Push if not exists
        auto it = itemSet.insert(edge);
        bool inserted = it.second;
        if (inserted) {
            itemStack.push(edge);
        }
    }
    DEdge UnuqEdgeStack::pop() {
        // Unsafe. Developer must know what he is doing.
        DEdge top = itemStack.top();
        itemStack.pop();
        itemSet.erase(top);
        return top;
    }
    bool UnuqEdgeStack::empty() const {
        return itemStack.empty();
    }

    Triangulation::~Triangulation() {
        for (auto p : points) {
            delete p;
        }
        for (auto t : triangles) {
            delete t;
        }
    }
    void Triangulation::setWorkBox(float startX, float startY, float endX, float endY) {
        // Work box
        workBox.min = { startX, startY };
        workBox.max = { endX, endY };
        superOffset = 4;

        // Super points
        points.emplace_back(new DPoint{ 0, startX, startY });
        points.emplace_back(new DPoint{ 1, startX, endY });
        points.emplace_back(new DPoint{ 2, endX, endY });
        points.emplace_back(new DPoint{ 3, endX, startY });

        createSuperStructure();
    }
    void Triangulation::createSuperStructure() {
        // Create 2 super triangles from first 4 points
        triangles.emplace_back(new DTriangle(0, points[0], points[1], points[2]));
        triangles.emplace_back(new DTriangle(1, points[2], points[3], points[0]));
        triangles[0]->link(triangles[1]);
        triangles[1]->link(triangles[0]);

        // Remember changes
        changedPoints.insert(points[0]);
        changedPoints.insert(points[1]);
        changedPoints.insert(points[2]);
        changedPoints.insert(points[3]);
        changedTriangles.insert(triangles[0]);
        changedTriangles.insert(triangles[1]);
    }
    void Triangulation::addPoint(float x, float y) {
        if (!workBox.contains(x, y)) {
            return;
        }

        //check existing point
        auto newPos = glm::vec2(x, y);
        for (auto point : points) {
            if (math::same(point->position, newPos)) {
                return;
            }
        }

        auto id = static_cast<uint32_t>(points.size());
        auto point = points.emplace_back(new DPoint{ id, x, y });
        changedPoints.insert(point);

        addPoint(point);
    }
    void Triangulation::addPoint(DPoint* point) {

        uint8_t hitCode = 0;
        auto triangleForSplit = findTriangle(point->position.x, point->position.y, hitCode);
        if (triangleForSplit == nullptr) {
            increaseError();
            return;
        }

        auto split = splitTriangle(triangleForSplit, hitCode, point);
        if (!split.ok) {
            increaseError();
            return;
        }

        for (auto& edge : split.edgesForCheck) {
            edgeStack.push(edge);
        }
        for (auto tr : split.changedTriangles) {
            if (tr) {
                changedTriangles.insert(tr);
            }
        }

        checkAndSwapEdges();
    }
    void Triangulation::checkAndSwapEdges() {
        while (!edgeStack.empty()) {

            // Get edge and unmark it
            DEdge edge = edgeStack.pop();

            // Search adjacent triangles for edge
            DTriangle* t1 = nullptr;
            DTriangle* t2 = nullptr;
            uint8_t trianglesForEdge = 0;
            for (auto triangle : triangles) {
                bool hasEdge =
                    triangle->hasPointId(edge.p0) &&
                    triangle->hasPointId(edge.p1);
                if (hasEdge) {
                    trianglesForEdge++;
                    if (t1 == nullptr) { t1 = triangle; }
                    else if (t2 == nullptr) { t2 = triangle; }
                }
            }

            if (trianglesForEdge == 0 || trianglesForEdge > 2) {
                /*
                    0: unknown edge
                    2: non manifold triangulation
                */
                increaseError();
                break;
            }
            if (trianglesForEdge == 1 || !t1 || !t2) {
                /*
                    1: it means border edge
                */
                continue;
            }

            bool isLegalEdge = legalEdge(edge, t1, t2);
            if (isLegalEdge) {
                continue;
            }

            bool ok = swapEdge(t1, t2);
            if (ok) {
                auto commonEdge = t1->getCommonEdge(t2);
                auto e1 = t1->getOppositeEdge(points[commonEdge.p0]);
                auto e2 = t1->getOppositeEdge(points[commonEdge.p1]);
                auto e3 = t2->getOppositeEdge(points[commonEdge.p0]);
                auto e4 = t2->getOppositeEdge(points[commonEdge.p1]);
                edgeStack.push(e1);
                edgeStack.push(e2);
                edgeStack.push(e3);
                edgeStack.push(e4);
                changedTriangles.insert(t1);
                changedTriangles.insert(t2);
            }
            else {
                increaseError();
                break;
            }
        }
    }
    bool Triangulation::legalEdge(const DEdge& edge, DTriangle* t1, DTriangle* t2) const {
        const auto& point = t2->getOppositePoint(edge)->position;
        bool contains = t1->circle.contains(point);
        return !contains;
    }
    bool Triangulation::swapEdge(DTriangle* t1, DTriangle* t2) {

        {
            bool someMad =
                util::hasErrors(t1) ||
                util::hasErrors(t2);
            if (someMad) {
                return false;
            }

            bool notLinked =
                !t1->linkedWith(t2) ||
                !t2->linkedWith(t1);
            if (notLinked) {
                return false;
            }
        }

        auto edge = t1->getCommonEdge(t2);
        auto v0 = points[edge.p0];
        auto v1 = points[edge.p1];
        auto v2 = t1->getOppositePoint(edge);
        auto v3 = t2->getOppositePoint(edge);

        // Adjacent triangle indices
        auto a11 = t1->getAdjacentTriangle(v1); // adjacent for edge {v2 - v0}
        auto a12 = t1->getAdjacentTriangle(v0); // adjacent for edge {v1 - v2}
        auto a21 = t2->getAdjacentTriangle(v1); // adjacent for edge {v0 - v3}
        auto a22 = t2->getAdjacentTriangle(v0); // adjacent for edge {v3 - v1}

        t1->setPoints(v2, v0, v3);
        t2->setPoints(v3, v1, v2);
        bool ok1 = t1->setAdjacent(a11, a21, t2);
        bool ok2 = t2->setAdjacent(a22, a12, t1);

        {
            bool someMad =
                !ok1 ||
                !ok2 ||
                util::hasErrors(t1) ||
                util::hasErrors(t2);
            if (someMad) {
                return false;
            }
        }
        return true;
    }
    void Triangulation::movePoint(size_t pointIndex, const glm::vec2& position) {
        if (pointIndex >= points.size()) {
            return;
        }

        auto point = points[pointIndex];
        point->position.x = math::trim(position.x, workBox.min.x, workBox.max.x);
        point->position.y = math::trim(position.y, workBox.min.y, workBox.max.y);

        changedPoints.insert(point);
        for (auto tr : triangles) {
            if (tr->hasPoint(point)) {
                tr->update(); 
                changedTriangles.insert(tr);
                auto id1 = tr->points[0]->id;
                auto id2 = tr->points[1]->id;
                auto id3 = tr->points[2]->id;
                edgeStack.push(DEdge{ id1, id2 });
                edgeStack.push(DEdge{ id2, id3 });
                edgeStack.push(DEdge{ id3, id1 });
            }
        }

        checkAndSwapEdges();
    }

    DTriangle* Triangulation::findTriangle(float x, float y, uint8_t& hitCode) {
        for (auto triangle : triangles) {
            if (triangle->contains(x, y, hitCode)) {
                return triangle;
            }
        }
        return nullptr;
    }
    SplitResult Triangulation::splitTriangle(DTriangle* triangle, uint8_t hitCode, DPoint* point) {

        int hitCount = 0;
        int edgeNum = 0;
        if (hitCode > 0) {
            if (hitCode & 0b0001) { hitCount++; edgeNum = 0; }
            if (hitCode & 0b0010) { hitCount++; edgeNum = 1; }
            if (hitCode & 0b0100) { hitCount++; edgeNum = 2; }
        }
        if (hitCount > 1) {
            // we hit the triangle's corner
            // there is an error here because we checked existing points before
            // no need to add triangles, just return an error
            return SplitResult{ false };
        }

        if (hitCode == 0) {
            return splitOneByInternalPoint(triangle, point);
        }

        DTriangle* adjacent = triangle->adjacent[edgeNum];
        if (adjacent) {
            return splitTwoByEdge(triangle, adjacent, point);
        }

        return splitOneByEdge(triangle, edgeNum, point);
    }
    SplitResult Triangulation::splitOneByInternalPoint(DTriangle* triangle, DPoint* point) {

        auto p0 = triangle->points[0];
        auto p1 = triangle->points[1];
        auto p2 = triangle->points[2];

        uint32_t tIndex0 = triangle->id;
        uint32_t tIndex1 = triangles.size();
        uint32_t tIndex2 = triangles.size() + 1;
        triangles[tIndex0]->setPoints(p0, p1, point);
        triangles.emplace_back(new DTriangle{ tIndex1, p1, p2, point });
        triangles.emplace_back(new DTriangle{ tIndex2, p2, p0, point });

        auto t0 = triangles[tIndex0];
        auto t1 = triangles[tIndex1];
        auto t2 = triangles[tIndex2];

        auto a0 = t0->adjacent[0];
        auto a1 = t0->adjacent[1];
        auto a2 = t0->adjacent[2];
        bool ok1 = t0->setAdjacent(a0, t1, t2);
        bool ok2 = t1->setAdjacent(a1, t2, t0);
        bool ok3 = t2->setAdjacent(a2, t0, t1);

        bool ok =
            (ok1 && ok2 && ok3) &&
            !util::hasErrors(t0) &&
            !util::hasErrors(t1) &&
            !util::hasErrors(t2) &&
            t0->hasPoint(point) &&
            t1->hasPoint(point) &&
            t2->hasPoint(point);

        auto e1 = t0->getOppositeEdge(point);
        auto e2 = t1->getOppositeEdge(point);
        auto e3 = t2->getOppositeEdge(point);

        return SplitResult{ ok, {t0, t1, t2}, {e1, e2, e3} };
    }
    SplitResult Triangulation::splitOneByEdge(DTriangle* t0, int edgeNum, DPoint* point) {
        // Split one edge in one triangle
        // We expect the adjacent triangle on this edge == null

        auto edge = t0->getEdge(edgeNum);
        auto v0 = points[edge.p0];
        auto v1 = points[edge.p1];
        auto v2 = t0->getOppositePoint(edge);

        auto A0 = t0->getAdjacentTriangle(v0);
        auto A1 = t0->getAdjacentTriangle(v1);
        auto A2 = t0->getAdjacentTriangle(v2); //expect null

        t0->setPoints(v2, v0, point);
        auto t1 = triangles.emplace_back(new DTriangle{ (uint32_t)triangles.size(), v1, v2, point });

        t0->setAdjacent(A1, nullptr, t1);
        t1->setAdjacent(A0, t0, nullptr);

        bool ok =
            (A2 == nullptr) &&
            !util::hasErrors(t0) &&
            !util::hasErrors(t1) &&
            t0->hasPoint(point) &&
            t1->hasPoint(point);

        auto e1 = t0->getOppositeEdge(point);
        auto e2 = t1->getOppositeEdge(point);
        return SplitResult{ ok, {t0, t1}, { e1, e2 } };
    }
    SplitResult Triangulation::splitTwoByEdge(DTriangle* triangle, DTriangle* adjacent, DPoint* point) {
        // Split one edge between two triangles
        DTriangle* t1 = triangle;
        DTriangle* t2 = adjacent;
        DTriangle* t3 = nullptr;
        DTriangle* t4 = nullptr;

        auto edge = t1->getCommonEdge(t2);
        auto v0 = points[edge.p0];
        auto v1 = points[edge.p1];
        auto v2 = t1->getOppositePoint(edge);
        auto v3 = t2->getOppositePoint(edge);

        auto A01 = t1->getAdjacentTriangle(v0);
        auto A11 = t1->getAdjacentTriangle(v1);
        auto A02 = t2->getAdjacentTriangle(v0);
        auto A12 = t2->getAdjacentTriangle(v1);

        t1->setPoints(v2, v0, point);
        t2->setPoints(v0, v3, point);
        t3 = triangles.emplace_back(new DTriangle{ (uint32_t)triangles.size(), v3, v1, point });
        t4 = triangles.emplace_back(new DTriangle{ (uint32_t)triangles.size(), v1, v2, point });

        t1->setAdjacent(A11, t2, t4);
        t2->setAdjacent(A12, t3, t1);
        t3->setAdjacent(A02, t4, t2);
        t4->setAdjacent(A01, t1, t3);

        bool ok =
            !util::hasErrors(t1) &&
            !util::hasErrors(t2) &&
            !util::hasErrors(t3) &&
            !util::hasErrors(t4) &&
            t1->hasPoint(point) &&
            t2->hasPoint(point) &&
            t3->hasPoint(point) &&
            t4->hasPoint(point);

        auto e1 = t1->getOppositeEdge(point);
        auto e2 = t2->getOppositeEdge(point);
        auto e3 = t3->getOppositeEdge(point);
        auto e4 = t4->getOppositeEdge(point);

        return SplitResult{ ok, {t1, t2, t3, t4}, {e1, e2, e3, e4} };
    }

    void Triangulation::updateView(Observer& observer) {
        observer.getUpdates(*this);
        changedPoints.clear();
        changedTriangles.clear();
    }
    void Triangulation::rebuild() {
        errors = 0;
        changedPoints.clear();
        changedTriangles.clear();

        for (auto t : triangles) {
            delete t;
        }
        triangles.clear();

        createSuperStructure();
        for (size_t i = superOffset; i < points.size(); i++) {
            addPoint(points[i]);
        }
    }
    bool Triangulation::isSuper(const DPoint* point) const {
        return point->id < superOffset;
    }
    bool Triangulation::isSuper(const DTriangle* tr) const {
        return
            isSuper(tr->points[0]) ||
            isSuper(tr->points[1]) ||
            isSuper(tr->points[2]);
    }
    bool Triangulation::isDelaynayConstrained(size_t tIndex) const {
        if (tIndex >= triangles.size()) {
            return false;
        }

        auto triangle = triangles[tIndex];
        for (auto adjacent : triangle->adjacent) {
            if (adjacent == nullptr) {
                continue;
            }

            auto edge = triangle->getCommonEdge(adjacent);
            bool legal = legalEdge(edge, triangle, adjacent);
            if (!legal) {
                return false;
            }
        }
        return true;
    }
    bool Triangulation::hasErrors() const {
        return errors > 0;
    }
    void Triangulation::increaseError() {
        errors++;
    }
}


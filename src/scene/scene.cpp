#include <iostream>
#include "platform/log.h"
#include "model/color.h"
#include "scene.h"

//todo: link point with triangles maybe?

namespace delaunay {

    namespace util {

        template <typename T>
        static void shiftLeft(T* values[3]) {
            auto first = values[0];
            for (auto i = 0; i < 2; i++) {
                values[i] = values[i + 1];
            }
            values[2] = first;
        }

        static void printTriangle(const Triangle* triangle) {
            if (triangle) {
                std::cout << "triangle id="
                    << triangle->id
                    << ": p=["
                    << triangle->point[0]->id << ", "
                    << triangle->point[1]->id << ", "
                    << triangle->point[2]->id << "] a=["
                    << triangle->adjacent[0] << ", "
                    << triangle->adjacent[1] << ", "
                    << triangle->adjacent[2] << "]\n";
            }
            else {
                std::cout << "Triangle is null" << std::endl;
            }
        }
        static void printPoint(const Point* point) {
            if (point) {
                std::cout << "point id="
                    << point->id << " x="
                    << point->position.x << ", y="
                    << point->position.y
                    << std::endl;
            }
            else {
                std::cout << "Point is null" << std::endl;
            }

        }
        static bool hasErrors(const Triangle* item) {
            const auto& p = item->point;
            bool samePoints =
                (p[0]->id == p[1]->id) ||
                (p[1]->id == p[2]->id) ||
                (p[2]->id == p[0]->id);
            if (samePoints) {
                return true;
            }

            const auto& a = item->adjacent;
            bool sameAdjacents =
                a[0] == a[1] ||
                a[0] == a[2] ||
                a[1] == a[2];
            if (sameAdjacents) {
                return true;
            }

            return false;
        }
        static int nearestIndex(const std::vector<Point*>& points, const glm::vec2& cursor, float& resultDistance) {
            if (points.empty()) {
                return -1;
            }

            auto index = 0;
            auto minDistance = glm::distance(points[0]->position, cursor);
            for (int i = 1; i < points.size(); i++) {
                auto& position = points[i]->position;
                float distance = glm::distance(position, cursor);
                if (distance < minDistance) {
                    minDistance = distance;
                    index = i;
                }
            }

            resultDistance = minDistance;
            return index;
        }
    }

    Circle::Circle(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
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
    bool Circle::contains(const glm::vec2& point) const {
        auto delta = point - center;
        return delta.x * delta.x + delta.y * delta.y <= radius * radius;
    }
    bool Box::contains(float x, float y) const {
        return
            min.x <= x && x <= max.x &&
            min.y <= y && y <= max.y;
    }
    Edge::Edge(uint32_t p0, uint32_t p1) {
        /*  if (p0 > p1) {
            std::swap(p0, p1);
        }*/
        this->p0 = p0;
        this->p1 = p1;
    }
    bool Edge::hasPoint(int pointId) const {
        return p0 == pointId || p1 == pointId;
    }

    Point::Point(uint32_t id, float x, float y) :
        id(id),
        position(x, y) { }

    Triangle::Triangle(uint32_t id, Point* p0, Point* p1, Point* p2) :
        id(id) {
        point[0] = p0;
        point[1] = p1;
        point[2] = p2;
        updateBox();
        updateCircle();
    }
    void Triangle::setPoints(Point* p0, Point* p1, Point* p2) {
        point[0] = p0;
        point[1] = p1;
        point[2] = p2;
        updateBox();
        updateCircle();
    }
    void Triangle::setAdjacent(Triangle* a0, Triangle* a1, Triangle* a2) {
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
    void Triangle::shiftPointFirst(int firstPointId) {
        if (!hasPoint(firstPointId)) {
            return;
        }

        while (point[0]->id != firstPointId) {
            util::shiftLeft(point);
            util::shiftLeft(adjacent);
        }
    }
    void Triangle::updateBox() {
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
    void Triangle::updateCircle() {
        circle = Circle{
            point[0]->position,
            point[1]->position,
            point[2]->position
        };
    }
    bool Triangle::hasPoint(int pointId) const {
        return
            point[0]->id == pointId ||
            point[1]->id == pointId ||
            point[2]->id == pointId;
    }
    bool Triangle::hasPoints(const Point& p1, const Point& p2) const {
        return
            hasPoint(p1.id) &&
            hasPoint(p2.id);
    }
    bool Triangle::link(Triangle* triangle) {
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
    bool Triangle::linkedWith(const Triangle* triangle) const {
        const bool linked =
            adjacent[0] == triangle ||
            adjacent[1] == triangle ||
            adjacent[2] == triangle;
        return linked;
    }
    bool Triangle::contains(float x, float y) const {

        if (!box.contains(x, y)) {
            return false;
        }

        const float eps = 0.000001f;
        auto pt = glm::vec3(x, y, 0);
        auto dir = glm::vec3(0, 0, 1);
        auto t0 = glm::vec3(point[0]->position, 0);
        auto t1 = glm::vec3(point[1]->position, 0);
        auto t2 = glm::vec3(point[2]->position, 0);

        /*
            Note that some of dot-s might be near 0. 
            It means we hit some edge and the point (x, y) is on the triangle's border
        */
        auto dot1 = glm::dot(glm::cross(t1 - t0, pt - t0), dir);
        auto dot2 = glm::dot(glm::cross(t2 - t1, pt - t1), dir);
        auto dot3 = glm::dot(glm::cross(t0 - t2, pt - t2), dir);

        bool insideCW = dot1 > eps && dot2 > eps && dot3 > eps;
        bool insideCCW = dot1 < -eps && dot2 < -eps && dot3 < -eps;
        return insideCW || insideCCW;
    }
    Point* Triangle::getOppositePoint(const Edge& edge) const {
        int matches = 0;
        if (edge.hasPoint(point[0]->id)) matches++;
        if (edge.hasPoint(point[1]->id)) matches++;
        if (edge.hasPoint(point[2]->id)) matches++;

        if (matches != 2) {
            auto msg = "[Triangle::getOppositePoint] Bad edge check";
            Log::warn(msg);
            throw std::runtime_error(msg);
        }

        if (!edge.hasPoint(point[0]->id))  return point[0];
        if (!edge.hasPoint(point[1]->id))  return point[1];
        if (!edge.hasPoint(point[2]->id))  return point[2];

        auto msg = "[Triangle::getOppositePoint] Bad edge result";
        Log::warn(msg);
        throw std::runtime_error(msg);
    }
    Point* Triangle::getOppositePoint(const Triangle* other) const {
        if (other == adjacent[0]) return point[2];
        if (other == adjacent[1]) return point[0];
        if (other == adjacent[2]) return point[1];
        return nullptr;
    }
    Triangle* Triangle::getOppositeTriangle(int pointId) const {
        if (pointId == point[0]->id) return adjacent[1];
        if (pointId == point[1]->id) return adjacent[2];
        if (pointId == point[2]->id) return adjacent[0];

        std::string msg = "Triangle does not have pointId=" + std::to_string(pointId);
        Log::warn(msg);
        return nullptr;
    }
    Edge Triangle::getCommonEdge(const Triangle* other) const {
        if (other == adjacent[0]) return Edge{ point[0]->id, point[1]->id };
        if (other == adjacent[1]) return Edge{ point[1]->id, point[2]->id };
        if (other == adjacent[2]) return Edge{ point[2]->id, point[0]->id };
        throw std::runtime_error("something goes wrong");
    }

    Triangulation::~Triangulation() {
        for (auto p : points) {
            delete p;
        }
        for (auto t : triangles) {
            delete t;
        }
    }
    bool Triangulation::hasErrors() const {
        return errors > 0;
    }
    void Triangulation::addPoint(float x, float y) {
        auto id = static_cast<uint32_t>(points.size());
        auto point = points.emplace_back(new Point{ id, x, y });
        changedPoints.emplace_back(point);
        addPoint(point);
    }
    void Triangulation::addPoint(Point* point) {
        auto pointsSize = points.size();
        if (pointsSize < 3) {
            return;
        }

        if (pointsSize > 2 && triangles.empty()) {
            auto superTriangle = new Triangle{ 0,
                points[0],
                points[1],
                points[2]
            };
            triangles.emplace_back(superTriangle);
            changedTriangles.insert(superTriangle);
            return;
        }

        auto triangleForSplit = findTriangle(point->position.x, point->position.y);
        if (triangleForSplit == nullptr) {
            Log::warn("SceneModel::addPoint(Point* point) - Triangle not found!");
            increaseError();
            return;
        }

        auto split = splitTriangle(triangleForSplit, point);
        if (!split.success) {
            Log::warn("SceneModel::addPoint(Point* point) - Bad split!");
            increaseError();
            return;
        }

        changedTriangles.insert(split.a);
        changedTriangles.insert(split.b);
        changedTriangles.insert(split.c);
        trianglesForCheck.push(split.a->id);
        trianglesForCheck.push(split.b->id);
        trianglesForCheck.push(split.c->id);
        checkDelaunay(trianglesForCheck, point);
        if (errors) {
            Log::warn("SceneModel::addPoint(Point* point) - Got errors after checkDelaunay()");
        }
    } 
    
    static uint64_t getEdgeKey(Triangle* t1, Triangle* t2) {
        auto edge = t1->getCommonEdge(t2);
        if (edge.p0 > edge.p1) {
            std::swap(edge.p0, edge.p1);
        }
        uint64_t result = 
            static_cast<uint64_t>(edge.p0) << 32 | 
            static_cast<uint64_t>(edge.p1);

        return result;
    }
    
    void Triangulation::movePoint(size_t index, const glm::vec2& position) {
        auto point = points[index];
        point->position = position;
        
        changedPoints.emplace_back(point);

        std::set<uint64_t> markedEdges;
        std::stack<uint32_t> checkNext; //triangle id-s
        for (auto tr : triangles) {
            if (tr->hasPoint(point->id)) {
                tr->updateBox();
                tr->updateCircle();
                checkNext.push(tr->id);
                changedTriangles.insert(tr);
                if (tr->adjacent[0]) {
                    markedEdges.insert(getEdgeKey(tr, tr->adjacent[0]));
                }
                if (tr->adjacent[1]) {
                    markedEdges.insert(getEdgeKey(tr, tr->adjacent[1]));
                }
                if (tr->adjacent[2]) {
                    markedEdges.insert(getEdgeKey(tr, tr->adjacent[2]));
                }
            }
        }

        while (!checkNext.empty()) {
            auto index = checkNext.top();
            auto target = triangles[index];
            checkNext.pop();

            for (auto adjacent : target->adjacent) {
                if (adjacent == nullptr) {
                    continue;
                }

                auto edgeKey = getEdgeKey(target, adjacent);
                bool marked = markedEdges.count(edgeKey) > 0;
                if (!marked) {
                    continue;
                }

                markedEdges.erase(edgeKey);

                auto checkPoint = adjacent->getOppositePoint(target);
                bool contains = target->circle.contains(checkPoint->position);
                if (!contains) {
                    continue;
                }
                
                bool ok = swapEdge(target, adjacent);
                if (ok) {
                    auto A1 = target->getOppositeTriangle(checkPoint->id);
                    auto A2 = adjacent->getOppositeTriangle(checkPoint->id);
                    markedEdges.insert(getEdgeKey(target, A1));
                    markedEdges.insert(getEdgeKey(adjacent, A2));

                    checkNext.push(target->id);
                    checkNext.push(adjacent->id);
                    changedTriangles.insert(target);
                    changedTriangles.insert(adjacent);
                    continue;
                }
                else {
                    increaseError();
                    return; //  return from function!
                }
            }
        }
    }
    void Triangulation::increaseError() {
        errors++;
    }
    Triangle* Triangulation::findTriangle(float x, float y) {
        for (auto triangle : triangles) {
            if (triangle->contains(x, y)) {
                return triangle;
            }
        }
        return nullptr;
    }
    SplitResult Triangulation::splitTriangle(Triangle* triangleForSplit, Point* point) {

        auto p0 = triangleForSplit->point[0];
        auto p1 = triangleForSplit->point[1];
        auto p2 = triangleForSplit->point[2];

        uint32_t tIndex0 = triangleForSplit->id;
        uint32_t tIndex1 = static_cast<int>(triangles.size());
        uint32_t tIndex2 = static_cast<int>(triangles.size() + 1);
        triangles[tIndex0]->setPoints(p0, p1, point);
        triangles.emplace_back(new Triangle{ tIndex1, p1, p2, point });
        triangles.emplace_back(new Triangle{ tIndex2, p2, p0, point });

        auto* t0 = triangles[tIndex0];
        auto* t1 = triangles[tIndex1];
        auto* t2 = triangles[tIndex2];

        auto* a0 = t0->adjacent[0];
        auto* a1 = t0->adjacent[1];
        auto* a2 = t0->adjacent[2];
        t0->setAdjacent(a0, t1, t2);
        t1->setAdjacent(a1, t2, t0);
        t2->setAdjacent(a2, t0, t1);

        {
            bool someMad =
                util::hasErrors(t0) ||
                util::hasErrors(t1) ||
                util::hasErrors(t2) ||
                !t0->hasPoint(point->id) ||
                !t1->hasPoint(point->id) ||
                !t2->hasPoint(point->id);
            if (someMad) {
                increaseError();
            }
        }

        return SplitResult{ true, t0, t1, t2 };
    }
    void Triangulation::checkDelaunay(std::stack<uint32_t>& checkNext, const Point* point) {
        while (!checkNext.empty()) {
            auto index = checkNext.top();
            auto target = triangles[index];
            checkNext.pop();

            auto adjacent = target->getOppositeTriangle(point->id);
            if (adjacent == nullptr) {
                continue;
            }

            auto checkPoint = adjacent->getOppositePoint(target);
            if (checkPoint == nullptr) {
                increaseError();
                return; //  return from function!
            }

            bool contains = target->circle.contains(checkPoint->position);
            if (!contains) {
                continue;
            }

            bool ok = swapEdge(target, adjacent);
            if (ok) {
                checkNext.push(target->id);
                checkNext.push(adjacent->id);
                changedTriangles.insert(target);
                changedTriangles.insert(adjacent);
                continue;
            }
            else {
                increaseError();
                return; //  return from function!
            }
        }
    } 
    bool Triangulation::hasDelaunay(const Triangle* target, const Triangle* adjacent) {

        auto edge = target->getCommonEdge(adjacent);
        auto point = adjacent->getOppositePoint(edge);
        if (target->circle.contains(point->position)) {
            //need swap
            return false;
        }

        //no need swap
        return true;
    }
    bool Triangulation::hasDelaunay(const Triangle* target, const Point* point) {
        return false;
    }

    bool Triangulation::swapEdge(Triangle* t1, Triangle* t2) {

        {
            bool someMad =
                util::hasErrors(t1) ||
                util::hasErrors(t2);
            if (someMad) {
                Log::warn("SceneModel::swapEdge() - Bad triangles before swap");
                return false;
            }

            bool notLinked =
                !t1->linkedWith(t2) ||
                !t2->linkedWith(t1);
            if (notLinked) {
                Log::warn("SceneModel::swapEdge() - Triangles are not linked");
                return false;
            }
        }

        auto commonEdge = t1->getCommonEdge(t2);
        t1->shiftPointFirst(commonEdge.p0);
        t2->shiftPointFirst(commonEdge.p1);

        auto v0 = points[commonEdge.p0];
        auto v1 = points[commonEdge.p1];
        auto v2 = t1->getOppositePoint(commonEdge);
        auto v3 = t2->getOppositePoint(commonEdge);
        t1->setPoints(v2, v0, v3);
        t2->setPoints(v3, v1, v2);

        // Adjacent triangle indices
        auto* a11 = t1->adjacent[2]; // adjacent for edge {v2 - v0}
        auto* a12 = t1->adjacent[1]; // adjacent for edge {v1 - v2}
        auto* a21 = t2->adjacent[1]; // adjacent for edge {v0 - v3}
        auto* a22 = t2->adjacent[2]; // adjacent for edge {v3 - v1}
        t1->setAdjacent(a11, a21, t2);
        t2->setAdjacent(a22, a12, t1);

        {
            bool someMad =
                util::hasErrors(t1) ||
                util::hasErrors(t2);
            if (someMad) {
                Log::warn("SceneModel::swapEdge() - Bad triangles after swap");
                return false;
            }
        }
        return true;
    }
    void Triangulation::updateView(Observer& observer) {
        observer.getUpdates(*this);
        changedPoints.clear();
        changedTriangles.clear();
    }
    void Triangulation::rebuild() {
        changedPoints.clear();
        changedTriangles.clear();

        for (auto t : triangles) {
            delete t;
        }
        triangles.clear();

        for (auto point : points) {
            addPoint(point);
        }
    }
    bool Triangulation::isSuper(const Point* point) {
        return point->id < 3;
    }
    bool Triangulation::isSuper(const Triangle* tr) {
        return
            isSuper(tr->point[0]) ||
            isSuper(tr->point[1]) ||
            isSuper(tr->point[2]);
    }
    
    void SceneView::getUpdates(const Triangulation& model) {
        
        //update points 
        pointMeshes.resize(model.points.size());
        for (auto point : model.changedPoints) {
            pointMeshes[point->id] = CircleMesh::createPoint(point->position);
        }

        //update triangles
        triangleMeshes.resize(3 * model.triangles.size());
        for (auto triangle : model.changedTriangles) {
            const auto& p0 = triangle->point[0]->position;
            const auto& p1 = triangle->point[1]->position;
            const auto& p2 = triangle->point[2]->position;
            
            size_t idx = static_cast<size_t>(triangle->id * 3);
            triangleMeshes[idx + 0].movePosition(p0, p1);
            triangleMeshes[idx + 1].movePosition(p1, p2);
            triangleMeshes[idx + 2].movePosition(p2, p0);

            bool super = Triangulation::isSuper(triangle);
            triangleMeshes[idx + 0].visible = !super;
            triangleMeshes[idx + 1].visible = !super;
            triangleMeshes[idx + 2].visible = !super;
        }
    }

    void Scene::init(const glm::vec2& viewSize) {

        const float w = viewSize.x;
        const float h = viewSize.y;
        auto p = 100.f;
        auto top = static_cast<float>(h) - p;
        auto right = static_cast<float>(w) - p;
        auto bottom = p;
        auto left = p;

        model.triangles.reserve(9);
        model.points.reserve(7);
        
        //3 points for super triangle
        model.addPoint(-w, -h);
        model.addPoint(2 * w, -h);
        model.addPoint(0.5f * w, 2.f * h);

        //4 rect points
        model.addPoint(left, bottom);
        model.addPoint(right, bottom);
        model.addPoint(right, top);
        model.addPoint(left, top);
        model.updateView(view);

        background.init(viewSize.x, viewSize.y);
        selectedPoint.mesh = CircleMesh::createPointSelected({ glm::vec2(0,0) });
    }
    void Scene::destroy() {
        background.destroy();
    }
    void Scene::addPoint(const glm::vec2& cursor) {
        model.addPoint(cursor.x, cursor.y);
        model.updateView(view);
    }
    void Scene::movePoint(const glm::vec2& cursor) {

        //skip unselected
        if (selectedPoint.index == -1) {
            return;
        }

        //skip super triangle
        if (selectedPoint.index < 3) {
            return;
        }

        auto newPosition = cursor - dragOffset;
        model.movePoint(selectedPoint.index, newPosition);
        model.updateView(view);

        auto position = model.points[selectedPoint.index]->position;
        selectedPoint.mesh.setPosition(position);
    }
    void Scene::selectPoint(const glm::vec2& cursor) {

        float distance = 0.f;
        auto index = util::nearestIndex(model.points, cursor, distance);
        if (distance >= 20.f) {
            selectedPoint.index = -1;
            return;
        }

        auto position = model.points[index]->position;
        selectedPoint.mesh.setPosition(position);
        selectedPoint.index = index;
        dragOffset = cursor - position;
    }
    void Scene::clearSelection() {
        selectedPoint.index = -1;
    }
    void Scene::rebuild() {
        model.rebuild();
        model.updateView(view);
    }
};

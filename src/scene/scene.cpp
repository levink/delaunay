#include <iostream>
#include <set>
#include "platform/log.h"
#include "model/color.h"
#include "scene.h"

//todo: link point with triangles maybe?



namespace delaunay {

    namespace util {
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
        static bool hasErrors(const Triangle& item) {
            const auto& p = item.point;
            bool samePoints =
                (p[0]->id == p[1]->id) ||
                (p[1]->id == p[2]->id) ||
                (p[2]->id == p[0]->id);
            if (samePoints) {
                return true;
            }

            const auto& a = item.adjacent;
            bool sameAdjacents =
                a[0] == a[1] ||
                a[0] == a[2] ||
                a[1] == a[2];
            if (sameAdjacents) {
                return true;
            }

            return false;
        }
    }
    

    Circle::Circle() : radius(0) { }
    Circle::Circle(float x, float y, float radius) : center(x, y), radius(radius) { }
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
    

    void SceneModel::addPoint(float x, float y) {
        auto id = static_cast<uint32_t>(points.size());
        auto point = points.emplace_back(new Point{ id, x, y });

        changedPoints.emplace_back(point->id);

        auto pointsSize = points.size();
        if (pointsSize < 3) {
            return;
        }

        if (pointsSize == 3) {
            //add super triangle
            triangles.emplace_back(new Triangle{ 0,
                points[0],
                points[1],
                points[2]
                });
            changedTriangles.insert(0);
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
        trsForCheck.push(split.a);
        trsForCheck.push(split.b);
        trsForCheck.push(split.c);
        checkDelaunayConstraint(trsForCheck, *point);

        if (errors) {
            Log::warn("SceneModel::addPoint(Point* point) - Got errors after checkDelaunayConstraint()");
        }
    }
    void SceneModel::movePoint(size_t index, const glm::vec2& position) {
        auto point = points[index];
        point->position = position;
        
        changedPoints.emplace_back((uint32_t)index);

        {
            auto adjacens = std::stack<int>();
            for (auto tr : triangles) {
                if (tr->hasPoint(point->id)) {
                    tr->updateBox();
                    adjacens.push(tr->id);
                    changedTriangles.insert(tr->id);
                }
            }
            checkDelaunayConstraint(adjacens, *point);
        }

        {
            auto pointId = point->id;
            auto checkSet = std::set<Triangle*>();
            auto checkList = std::vector<Triangle*>();
            for (auto tr : triangles) {
                if (tr->hasPoint(pointId)) {
                    checkSet.insert(tr);
                }
            }
            if (checkSet.empty()) {
                return;
            }

            auto front = *checkSet.begin();
            checkList.reserve(checkSet.size());
            checkList.emplace_back(front);
            checkSet.erase(front);

            while (!checkSet.empty()) {
                auto back = checkList.back();
                for (auto item : checkSet) {
                    if (item->linkedWith(*back)) {
                        checkList.emplace_back(item);
                        break;
                    }
                }
                checkSet.erase(checkList.back());
            }

            for (int i = 0; i < checkList.size(); i++) {
                int j = (i + 1) % checkList.size();
                auto t1 = checkList[i];
                auto t2 = checkList[j];
                if (hasDelaunayConstraint(t1, t2)) {
                    continue;
                }
                
                auto swap = swapEdge(*t1, *t2);
                if (swap.success) {
                    changedTriangles.insert(swap.t1);
                    changedTriangles.insert(swap.t2);

                    bool has1 = t1->hasPoint(pointId);
                    bool has2 = t2->hasPoint(pointId);
                    if (!has1) {
                        checkList.erase(checkList.begin() + i);
                    } 
                    else if (!has2) {
                        checkList.erase(checkList.begin() + j);
                    }
                    else {
                        Log::warn("SceneModel::movePoint(...) - Triangle swap error 1!");
                        increaseError();
                        break;
                    }
                    i--;
                }
                else {
                    Log::warn("SceneModel::movePoint(...) - Triangle swap error 2!");
                    increaseError();
                    break;
                }

            }
        }
    }
    void SceneModel::increaseError() {
        errors++;
    }
    Triangle* SceneModel::findTriangle(float x, float y) {
        for (auto triangle : triangles) {
            if (triangle->contains(x, y)) {
                return triangle;
            }
        }
        return nullptr;
    }
    SplitResult SceneModel::splitTriangle(Triangle* triangleForSplit, const Point* point) {

        auto p0 = triangleForSplit->point[0];
        auto p1 = triangleForSplit->point[1];
        auto p2 = triangleForSplit->point[2];

        const int tIndex0 = triangleForSplit->id;
        const int tIndex1 = static_cast<int>(triangles.size());
        const int tIndex2 = static_cast<int>(triangles.size() + 1);
        triangles[tIndex0]->setPoints(p0, p1, point);
        triangles.emplace_back(new Triangle{ tIndex1, p1, p2, point });
        triangles.emplace_back(new Triangle{ tIndex2, p2, p0, point });

        auto& t0 = *triangles[tIndex0];
        auto& t1 = *triangles[tIndex1];
        auto& t2 = *triangles[tIndex2];

        int a0 = t0.adjacent[0];
        int a1 = t0.adjacent[1];
        int a2 = t0.adjacent[2];
        t0.setAdjacent(a0, t1.id, t2.id);
        t1.setAdjacent(a1, t2.id, t0.id);
        t2.setAdjacent(a2, t0.id, t1.id);
        linkTriangleByIndex(a0, t0);
        linkTriangleByIndex(a1, t1);
        linkTriangleByIndex(a2, t2);

        {
            bool someMad =
                util::hasErrors(t0) ||
                util::hasErrors(t1) ||
                util::hasErrors(t2) ||
                !t0.hasPoint(point->id) ||
                !t1.hasPoint(point->id) ||
                !t2.hasPoint(point->id);
            if (someMad) {
                increaseError();
            }
        }

        return SplitResult{ true, t0.id, t1.id, t2.id };
    }
    SwapResult SceneModel::swapEdge(Triangle& t1, Triangle& t2) {

        {
            bool someMad =
                util::hasErrors(t1) ||
                util::hasErrors(t2);
            if (someMad) {
                Log::warn("SceneModel::swapEdge() - Bad triangles before swap");
                return SwapResult{ false };
            }

            bool notLinked =
                !t1.linkedWith(t2) ||
                !t2.linkedWith(t1);
            if (notLinked) {
                Log::warn("SceneModel::swapEdge() - Triangles are not linked");
                return SwapResult{ false };
            }
        }

        auto commonEdge = t1.getCommonEdge(t2);
        t1.shiftPointFirst(commonEdge.v0);
        t2.shiftPointFirst(commonEdge.v1);

        auto v0 = points[commonEdge.v0];
        auto v1 = points[commonEdge.v1];
        auto v2 = points[t1.getOppositePoint(commonEdge)];
        auto v3 = points[t2.getOppositePoint(commonEdge)];
        t1.setPoints(v2, v0, v3);
        t2.setPoints(v3, v1, v2);

        // Adjacent triangle indices
        int a11 = t1.adjacent[2]; // adjacent for edge {v2 - v0}
        int a12 = t1.adjacent[1]; // adjacent for edge {v1 - v2}
        int a21 = t2.adjacent[1]; // adjacent for edge {v0 - v3}
        int a22 = t2.adjacent[2]; // adjacent for edge {v3 - v1}
        t1.setAdjacent(a11, a21, t2.id);
        t2.setAdjacent(a22, a12, t1.id);
        linkTriangleByIndex(a12, t2);
        linkTriangleByIndex(a21, t1);

        {
            bool someMad =
                util::hasErrors(t1) ||
                util::hasErrors(t2);
            if (someMad) {
                Log::warn("SceneModel::swapEdge() - Bad triangles after swap");
                return SwapResult{ false };
            }
        }

        return SwapResult{
            true,
            t1.id,
            t2.id
        };
    }
    void SceneModel::linkTriangleByIndex(int adjacentIndex, const Triangle& target) {
        if (adjacentIndex == -1) {
            return;
        }


        bool linked = triangles[adjacentIndex]->link(target);
        if (!linked) {
            increaseError();
        }
    }
    void SceneModel::checkDelaunayConstraint(std::stack<int>& trianglesForCheck, const Point& point) {

        while (!trianglesForCheck.empty()) {
            auto splitIndex = trianglesForCheck.top();
            trianglesForCheck.pop();

            if (splitIndex == -1) {
                continue;
            }

            auto splitted = triangles[splitIndex];
            auto oppositeIndex = splitted->getOppositeTriangle(point.id);
            if (oppositeIndex == -1) {
                continue;
            }

            auto opposite = triangles[oppositeIndex];
            auto hull = splitted->getHull(opposite);
            auto concave = !hull.isConvex();
            if (concave) {
                continue;
            }

            auto circle = opposite->getCircle();
            bool hasDelaunayConstraint = !circle.contains(point.position);
            if (hasDelaunayConstraint) {
                continue;
            }

            auto swap = swapEdge(*splitted, *opposite);
            if (swap.success) {
                changedTriangles.insert(swap.t1);
                changedTriangles.insert(swap.t2);
                trianglesForCheck.push(swap.t1);
                trianglesForCheck.push(swap.t2);
            }
            else {
                increaseError();
                break;
            }
        }
    }
    bool SceneModel::hasDelaunayConstraint(const Triangle* t1, const Triangle* t2) {

        Hull hull = t1->getHull(t2);
        bool concave = !hull.isConvex();
        if (concave) {
            //skip concave hulls
            //no need swap
            return true;
        }

        Edge edge = t1->getCommonEdge(*t2);
        const auto& p1 = points[t1->getOppositePoint(edge)]->position;
        const auto& p2 = points[t2->getOppositePoint(edge)]->position;

        Circle circle1 = t1->getCircle();
        if (circle1.contains(p2)) {
            //need swap
            return false;
        }

        Circle circle2 = t2->getCircle();
        if (circle2.contains(p1)) {
            //need swap
            return false;
        }

        //no need swap
        return true;
    }
    int SceneModel::nearestPointIndex(const glm::vec2& cursor, float& resultDistance) {
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
    void SceneModel::clearChanged() {
        changedPoints.clear();
        changedTriangles.clear();
    }


    void SceneView::updateView(const SceneModel& model) {
        
        //update points 
        pointMeshes.resize(model.points.size());
        for (auto index : model.changedPoints) {
            auto point = model.points[index];
            pointMeshes[index] = CircleMesh::createPoint(point->position);
        }

        //update triangles
        triangleMeshes.resize(3 * model.triangles.size());
        for (auto index : model.changedTriangles) {
            auto triangle = model.triangles[index];
            const auto& p0 = triangle->point[0]->position;
            const auto& p1 = triangle->point[1]->position;
            const auto& p2 = triangle->point[2]->position;

            size_t idx = static_cast<size_t>(index * 3);
            triangleMeshes[idx + 0].movePosition(p0, p1);
            triangleMeshes[idx + 1].movePosition(p1, p2);
            triangleMeshes[idx + 2].movePosition(p2, p0);
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

        model.triangles.reserve(5);
        model.points.reserve(5);
        model.addPoint(100, 100);
        model.addPoint(700, 100);
        model.addPoint(400, 500);
        model.addPoint(left + 2 * p, 2 * p);
        model.addPoint(right - 2 * p, 2 * p);
        view.updateView(model);
        model.clearChanged();

        background.init(viewSize.x, viewSize.y);
        selectedPoint.mesh = CircleMesh::createPointSelected({ glm::vec2(0,0) });
    }
    void Scene::destroy() {
        background.destroy();
    }
    void Scene::addPoint(const glm::vec2& cursor) {
        model.addPoint(cursor.x, cursor.y);
        view.updateView(model);
        model.clearChanged();
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
        view.updateView(model);
        model.clearChanged();

        auto position = model.points[selectedPoint.index]->position;
        selectedPoint.mesh.setPosition(position);
    }
    void Scene::selectPoint(const glm::vec2& cursor) {

        float distance = 0.f;
        auto index = model.nearestPointIndex(cursor, distance);
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
   

    /*void SceneModel::saveToFile() {
        std::ofstream file("../../findTriangle.txt");

        for (auto p : points) {
            file << p->position.x << " "
                << p->position.y << "\n";
        }

        for (auto t : triangles) {
            file << t->id << ": p=["
                << t->point[0]->id << " "
                << t->point[1]->id << " "
                << t->point[2]->id << "] a=["
                << t->adjacent[0] << " "
                << t->adjacent[1] << " "
                << t->adjacent[2] << "]\n";
        }

        file.close();
    }*/
};

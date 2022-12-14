#include <iostream>
#include "platform/log.h"
#include "model/color.h"
#include "scene.h"

namespace delaunay {

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

    void SceneModel::init(float w, float h) {
        auto padding = 100.f;
        auto top = static_cast<float>(h) - padding;
        auto right = static_cast<float>(w) - padding;
        auto bottom = padding;
        auto left = padding;

        constexpr int size = 4;
        points.reserve(3 + size);
        
        //3 points for supertriangle
        points.emplace_back(Point(0, -w, 0));
        points.emplace_back(Point(1, 2 * w, 0));
        points.emplace_back(Point(2, w / 2, 4 * h));

        //4 points for view quad
        points.emplace_back(Point(3, left, bottom));
        points.emplace_back(Point(4, right, bottom));
        points.emplace_back(Point(5, right, top));
        points.emplace_back(Point(6, left, top));
    }
    void SceneModel::addPoint(float x, float y) {
        int lastIndex = static_cast<int>(points.size());
        points.emplace_back(lastIndex, x, y);
    }
    void SceneModel::movePoint(size_t index, const glm::vec2& position) {
        
        auto& point = points[index];
        point.setPosition(position);

        for (size_t i = 0; i < triangles.size(); i++) {
            auto& tr = triangles[i];
            if (tr.has(point)) {
                tr.update(point);
            }
        }
    }
    void SceneModel::triangulate() {
        triangles.clear();
        
        // add super triangle
        triangles.emplace_back(Triangle { 0,
            points[0],
            points[1],
            points[2]
        });

        // add inner points
        for (auto& point : points) {
            if (super(point)) {
                continue;
            }

            auto triangleForSplit = findTriangle(point.getPosition());
            if (triangleForSplit == nullptr) {
                Log::warn("Triangle not found!");
                continue;
            }

            auto trianglesForCheck = split(*triangleForSplit, point);
            swapEdges(trianglesForCheck, point);
        }
    }
    bool SceneModel::super(const Point& point) const {
        return point.index < 3;
    }
    bool SceneModel::super(const Triangle& tr) const {
        return
            super(tr.point[0]) ||
            super(tr.point[1]) ||
            super(tr.point[2]);
    }
    Triangle* SceneModel::findTriangle(const glm::vec2& point) {
        for (auto& triangle : triangles) {
            if (triangle.contains(point)) { 
                return &triangle;
            }
        }
        return nullptr;
    }
    std::stack<int> SceneModel::split(Triangle triangleForSplit, Point point) {

        const auto& p0 = triangleForSplit.point[0];
        const auto& p1 = triangleForSplit.point[1];
        const auto& p2 = triangleForSplit.point[2];

        const int tIndex0 = triangleForSplit.index;
        const int tIndex1 = static_cast<int>(triangles.size());
        const int tIndex2 = tIndex1 + 1;
        triangles[triangleForSplit.index] = Triangle{ tIndex0, p0, p1, point };
        triangles.emplace_back(Triangle{ tIndex1, p1, p2, point });
        triangles.emplace_back(Triangle{ tIndex2, p2, p0, point });

        auto& t0 = triangles[tIndex0];
        auto& t1 = triangles[tIndex1];
        auto& t2 = triangles[tIndex2];

        checkError(t0);
        checkError(t1);
        checkError(t2);

        t0.adjacent[0] = triangleForSplit.adjacent[0];
        t0.adjacent[1] = t1.index;
        t0.adjacent[2] = t2.index;

        t1.adjacent[0] = triangleForSplit.adjacent[1];
        t1.adjacent[1] = t2.index;
        t1.adjacent[2] = t0.index;

        t2.adjacent[0] = triangleForSplit.adjacent[2];
        t2.adjacent[1] = t0.index;
        t2.adjacent[2] = t1.index;

        if (triangleForSplit.adjacent[0] != -1) {
            triangles[triangleForSplit.adjacent[0]].link(t0);
        }

        if (triangleForSplit.adjacent[1] != -1) {
            triangles[triangleForSplit.adjacent[1]].link(t1);
        }

        if (triangleForSplit.adjacent[2] != -1) {
            triangles[triangleForSplit.adjacent[2]].link(t2);
        }

        return std::stack<int>({
            t0.index,
            t1.index,
            t2.index
        });
    }
    void SceneModel::swapEdges(std::stack<int>& trianglesForCheck, const Point& point) {
        while (!trianglesForCheck.empty()) {
            auto splitIndex = trianglesForCheck.top();
            trianglesForCheck.pop();

            if (splitIndex == -1) {
                continue;
            }

            auto& splitted = triangles[splitIndex];
            auto oppositeIndex = splitted.getOpposite(point.index);
            if (oppositeIndex == -1) {
                continue;
            }

            auto& opposite = triangles[oppositeIndex];
            auto hull = splitted.getHull(opposite);
            auto concave = !hull.isConvex();
            if (concave) {
                continue;
            }

            auto circle = opposite.getCircle();
            auto delaunay = !circle.contains(point.getPosition());
            if (delaunay) {
                continue;
            }

            auto swap = swapEdge(point, splitted, opposite);
            if (swap.success) {
                trianglesForCheck.push(swap.first);
                trianglesForCheck.push(swap.second);
            }
        }
    }
    void SceneModel::checkError(const Triangle& triangle) {
        if (triangle.hasError()) {
            Log::warn("[Triangle::checkError] Triangle is bad");
        }
    };
    SwapResult SceneModel::swapEdge(const Point& splitPoint, Triangle& old1, Triangle& old2) {
        checkError(old1);
        checkError(old2);

        if (!old1.linkedWith(old2) || !old2.linkedWith(old1)) {
            Log::warn("[swapEdge] Triangles are not linked!");
            return SwapResult{ false };
        }

        auto commonEdge = old1.getCommonEdge(old2);
        old1.setFirst(commonEdge.v0);
        old2.setFirst(commonEdge.v1);

        auto& p1 = points[commonEdge.v0];
        auto& p2 = points[commonEdge.v1];
        auto& p3 = points[old1.getOppositePoint(commonEdge)];
        auto& p4 = points[old2.getOppositePoint(commonEdge)];

        auto new1 = Triangle{ old1.index, p1, p4, p3 };
        auto new2 = Triangle{ old2.index, p2, p3, p4 };
        checkError(new1);
        checkError(new2);

        new1.adjacent[0] = old2.adjacent[1];
        new1.adjacent[1] = old2.index;
        new1.adjacent[2] = old1.adjacent[2];

        new2.adjacent[0] = old1.adjacent[1];
        new2.adjacent[1] = old1.index;
        new2.adjacent[2] = old2.adjacent[2];

        if (new1.adjacent[0] != -1) triangles[new1.adjacent[0]].link(new1);
        if (new1.adjacent[1] != -1) triangles[new1.adjacent[1]].link(new1);
        if (new1.adjacent[2] != -1) triangles[new1.adjacent[2]].link(new1);

        if (new2.adjacent[0] != -1) triangles[new2.adjacent[0]].link(new2);
        if (new2.adjacent[1] != -1) triangles[new2.adjacent[1]].link(new2);
        if (new2.adjacent[2] != -1) triangles[new2.adjacent[2]].link(new2);

        triangles[old1.index] = new1;
        triangles[old2.index] = new2;

        checkError(new1);
        checkError(new2);

        return SwapResult{
                true,
                new1.index,//, new1.adjacent[2]},
                new2.index //, new2.adjacent[0]}
        };
    }


    void SceneView::init(const SceneModel& model) {
        pointsMesh.clear();
        pointsMesh.reserve(model.points.size() - 3);
        for (auto& point : model.points) {
            if (model.super(point)) {
                continue;
            }
            addPoint(point);
        }

        trianglesMesh.clear();
        trianglesMesh.reserve(3 * model.triangles.size());
        for (auto& triangle : model.triangles) {
            if (model.super(triangle)) {
                continue;
            }

            const auto& p0 = triangle.point[0].getPosition();
            const auto& p1 = triangle.point[1].getPosition();
            const auto& p2 = triangle.point[2].getPosition(); 

            trianglesMesh.push_back(LineMesh(p0, p1));
            trianglesMesh.push_back(LineMesh(p1, p2));
            trianglesMesh.push_back(LineMesh(p2, p0));
        }

        selectedPoint.active = false;
        selectedPoint.mesh = CircleMesh::createPointSelected({ glm::vec2(0,0) });
    }
    void SceneView::addPoint(const Point& point) {
        pointsMesh.push_back(CircleMesh::createPoint(
            point.getPosition()
        ));
    }
    void SceneView::updateSelected(size_t index, const glm::vec2& position) {
        pointsMesh[index].setPosition(position);
        selectedPoint.mesh.setPosition(position);
    }
    void SceneView::updateTriangles(const SceneModel& model) {

        auto meshSize = 3 * model.triangles.size();
        trianglesMesh.resize(meshSize);

        size_t index = 0;
        for (auto& triangle : model.triangles) {

            if (model.super(triangle)) {
                continue;
            }

            const auto& p0 = triangle.point[0].getPosition();
            const auto& p1 = triangle.point[1].getPosition();
            const auto& p2 = triangle.point[2].getPosition();

            trianglesMesh[index + 0].move(p0, p1);
            trianglesMesh[index + 1].move(p1, p2);
            trianglesMesh[index + 2].move(p2, p0);

            index += 3;
        }
        trianglesMesh.resize(index);
    }


    void Scene::init(const glm::vec2& viewSize) {
        model.init(viewSize.x, viewSize.y);
        model.triangulate();
        view.init(model);
    }
    void Scene::addPoint(const glm::vec2& cursor) {

        model.addPoint(cursor.x, cursor.y);
        model.triangulate();

        auto& point = model.points.back();
        view.addPoint(point);
        view.updateTriangles(model);

        auto& position = point.getPosition();
        std::wcout << "Point "
            << point.index << " "
            << position.x << " "
            << position.y << std::endl;
    } 
    void Scene::deletePoint(const glm::vec2 &cursor) {

    }
    int Scene::nearestPoint(const glm::vec2& cursor, float& resultDistance) {
        auto& points = model.points;
        if (points.empty()) {
            return -1;
        }

        auto index = 0;
        auto minDistance = glm::distance(points[0].getPosition(), cursor);
        for (int i = 1; i < points.size(); i++) {
            auto& position = points[i].getPosition();
            float distance = glm::distance(position, cursor);
            if (distance < minDistance) {
                minDistance = distance;
                index = i;
            }
        }

        resultDistance = minDistance;
        return index;
    }
    void Scene::selectPoint(const glm::vec2& cursor) {
        
        float distance = 0.f;
        model.selectedPointIndex = nearestPoint(cursor, distance);
        if (model.selectedPointIndex == -1 || distance >= 20.f) {
            view.selectedPoint.active = false;
            return;
        }
        
        auto& point = model.points[model.selectedPointIndex];
        model.dragOffset = cursor - point.getPosition();
        view.selectedPoint.active = true;
        view.selectedPoint.mesh.setPosition(point.getPosition());
        
        auto& offset = model.dragOffset;
        std::wcout << "Selected: " 
            << "index = " << model.selectedPointIndex  << ", " 
            << "offset = [" << offset.x << ", " << offset.y << "] "
            << std::endl;
    }
    void Scene::movePoint(const glm::vec2& cursor) {

        if (model.selectedPointIndex == -1) {
            return;
        }

        auto index = model.selectedPointIndex;
        auto newPosition = cursor - model.dragOffset;
        model.movePoint(index, newPosition);
        model.triangulate();

        auto viewIndex = static_cast<size_t>(index) - 3;
        view.updateSelected(viewIndex, newPosition);
        view.updateTriangles(model);
    }
    void Scene::clearSelection() {
        model.selectedPointIndex = -1;
        view.selectedPoint.active = false;

        model.triangulate();
        view.updateTriangles(model);
    }

};






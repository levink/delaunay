#include <iostream>
#include "platform/log.h"
#include "model/color.h"
#include "scene.h"

namespace scene_version_1 {

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

        //visible points
        points.clear();
        points.reserve(5 + 3); //5 - below, 3 - for supertriangle
        points.emplace_back(0, left, bottom);
        points.emplace_back(1, right, bottom);
        points.emplace_back(2, right, top);
        points.emplace_back(3, left, top);
        points.emplace_back(4, 375, 25);
    }
    void SceneModel::triangulate() {
        auto koefs = normalizePoints();
        auto tr = addSuperTriangle();
        addInnerPoints(tr);
        removeSuperTriangle(tr);
        restorePoints(koefs);
    }

    Normalization SceneModel::normalizePoints() {
        Normalization normalization;
        if (points.empty()) {
            return normalization;
        }

        auto min = points[0].getPosition();
        auto max = points[0].getPosition();
        for (auto& p : points) {
            auto pos = p.getPosition();
            min.x = std::min(min.x, pos.x);
            min.y = std::min(min.y, pos.y);
            max.x = std::max(max.x, pos.x);
            max.y = std::max(max.y, pos.y);
        }

        normalization.scale = std::max(max.x - min.x, max.y - min.y);
        if (normalization.scale == 0) {
            normalization.scale = std::max(max.x, max.y);
        }
        normalization.offset = min;


        for (auto& p : points) {
            auto value = (p.getPosition() - normalization.offset) / normalization.scale;
            p.setPosition(value);
        }

        return normalization;
    }
    void SceneModel::restorePoints(const Normalization& normalization) {
        if (points.empty()) {
            return;
        }

        for (auto& p : points) {
            auto value = p.getPosition() * normalization.scale + normalization.offset;
            p.setPosition(value);
        }

        for (auto& t : triangles) {
            auto& p0 = t.point[0];
            auto& p1 = t.point[1];
            auto& p2 = t.point[2];

            p0 = points[p0.index];
            p1 = points[p1.index];
            p2 = points[p2.index];
        }
    }

    TriangleIndex SceneModel::addSuperTriangle() {
        //super triangle points
        auto index0 = points.size();
        auto index1 = index0 + 1;
        auto index2 = index0 + 2;
        points.emplace_back(index0, -100, -100);
        points.emplace_back(index1, 100, -100);
        points.emplace_back(index2, 0, 100);

        //super triangle
        triangles.clear();
        triangles.emplace_back(
            static_cast<int>(0),
            points[index0],
            points[index1],
            points[index2]
        );
        return TriangleIndex{ 
            static_cast<unsigned>(index0), 
            static_cast<unsigned>(index1), 
            static_cast<unsigned>(index2) 
        };
    }
    void SceneModel::removeSuperTriangle(const TriangleIndex& tr) {

        std::vector<int> forDelete;
        for (int i = triangles.size() - 1; i >= 0; i--) {
            auto& t = triangles[i];
            auto canDelete = 
                t.has(tr.pointIndex0) ||
                t.has(tr.pointIndex1) ||
                t.has(tr.pointIndex2);

            if (canDelete) {
                forDelete.push_back(i);
            }
        }

        for (auto index : forDelete) {
            triangles.erase(triangles.begin() + index);
            for (auto& t : triangles) {
                t.replaceAdjacent(index, -1);
                t.decrementIndices(index);
            }
        }

        points.erase(points.begin() + tr.pointIndex2);
        points.erase(points.begin() + tr.pointIndex1);
        points.erase(points.begin() + tr.pointIndex0);
    }

    void SceneModel::addInnerPoints(const TriangleIndex& superTriangle) { 
        for (auto& point : points) {
            if (superTriangle.has(point.index)) {
                continue;
            }

            auto triangleIndexForSplit = findTriangle(point.getPosition());
            if (triangleIndexForSplit == -1) {
                Log::warn("Triangle not found!");
                continue;
            }

            auto triangleForSplit = triangles[triangleIndexForSplit];
            auto trianglesForCheck = split(triangleForSplit, point);
            swapEdges(trianglesForCheck, point);
        }
    }
    int SceneModel::findTriangle(const glm::vec2& point) {
        for (size_t i = 0; i < triangles.size(); i++) {
            auto& triangle = triangles[i];
            if (triangle.contains(point)) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    std::stack<int> SceneModel::split(Triangle& triangleForSplit, Point& point) {
        
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

        t0.checkError();
        t1.checkError();
        t2.checkError();

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
    SceneModel::SwapResult SceneModel::swapEdge(const Point& splitPoint, Triangle& old1, Triangle& old2) {
        old1.checkError();
        old2.checkError();

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
        new1.checkError();
        new2.checkError();

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

        new1.checkError();
        new2.checkError();

        return SwapResult{
                true,
                new1.index,//, new1.adjacent[2]},
                new2.index //, new2.adjacent[0]}
        };
    }
    

    void Scene::initScene(const glm::vec2& viewSize) {
       
        model.init(viewSize.x, viewSize.y);
        model.triangulate();

        updateView();
    }
    void Scene::updateView() {
        view.pointsMesh.reserve(model.points.size());
        view.pointsMesh.clear();
        for(auto& point : model.points) {
            const auto& pos = point.getPosition();
            const auto& mesh = createPointMesh(pos);
            view.pointsMesh.push_back(mesh);
        }
    
        view.trianglesMesh.reserve(3 * model.triangles.size());
        view.trianglesMesh.clear();
    
        for (auto& triangle : model.triangles) {

            auto p0 = triangle.point[0].getPosition();
            auto p1 = triangle.point[1].getPosition();
            auto p2 = triangle.point[2].getPosition();

            view.trianglesMesh.push_back(createLineMesh(p0, p1));
            view.trianglesMesh.push_back(createLineMesh(p1, p2));
            view.trianglesMesh.push_back(createLineMesh(p2, p0));
        }
    }
    void Scene::addPoint(const glm::vec2& cursor) {
        int lastIndex = static_cast<int>(model.points.size());
        model.points.emplace_back(lastIndex, cursor.x, cursor.y);

        std::cout << "Point " << lastIndex << " " << cursor.x << ", " << cursor.y << std::endl;

        auto pos = model.points.back().getPosition();
        auto mesh = createPointMesh(pos);
        view.pointsMesh.emplace_back(mesh);

        model.triangulate();
        updateView();
    }
    void Scene::movePoint(const glm::vec2& cursor) {
       /* if (selectedPoint == -1) {
            return;
        }

        auto position = cursor - dragDrop;
        points[selectedPoint].setPosition(position);

        triangulate();
        updateView();*/
    }
    void Scene::deletePoint(const glm::vec2 &cursor) {

    }
    void Scene::selectPoint(const glm::vec2& cursor) {

      /*  if (points.empty()) {
            return;
        }

        auto pos = points[0].getPosition();
        auto minDistance = glm::distance(pos, cursor);
        dragDrop = cursor - pos;
        auto index = 0;

        for(int i = 1; i < points.size(); i++) {
            auto& point = points[i];
            auto pointPos = point.getPosition();
            float distance = glm::distance(pointPos, cursor);
            if (distance < minDistance) {
                minDistance = distance;
                index = i;
                dragDrop = cursor - pointPos;
            }
        }

        if (selectedPoint != -1) {
            pointsMesh[selectedPoint].color = Color::teal;
        }

        if (minDistance >= 20.f) {
            selectedPoint = -1;
        }
        else {
            selectedPoint = index;
            pointsMesh[selectedPoint].color = Color::yellow;
        }

        std::string msg = "Selected point = " + std::to_string(selectedPoint);
        Log::out(msg);*/
    }
    void Scene::clearSelection() {
      /*  if (selectedPoint != -1) {
            pointsMesh[selectedPoint].color = Color::teal;
        }
        selectedPoint = -1;
        dragDrop.x = dragDrop.y = 0;*/
    }

    const CircleMesh* Scene::getSelectedCircle() {
        if (model.selectedTriangle == -1) {
            return nullptr;
        }

        return nullptr;//&selectedCircleMesh;
        //todo this
    }

    CircleMesh Scene::createPointMesh(const glm::vec2& point) {
        return CircleMesh(point, 7.f, true, Color::teal);
    }
    CircleMesh Scene::createCircleMesh(const Circle& circle) {
        return CircleMesh {circle.center, circle.radius, false, Color::teal };
    }
    LineMesh Scene::createLineMesh(const glm::vec2& start, const glm::vec2& end) {
        return LineMesh::create(start, end, Color::orange, 3.5f);
    }
};






#include <iostream>
#include "scene/scene.h"
#include "view/color.h"

using namespace delaunay;

namespace util {
    static void printTriangle(const DTriangle* triangle) {
        if (triangle) {
            std::cout << "triangle id="
                << triangle->id
                << ": p=["
                << triangle->points[0]->id << ", "
                << triangle->points[1]->id << ", "
                << triangle->points[2]->id << "] a=["
                << triangle->adjacent[0] << ", "
                << triangle->adjacent[1] << ", "
                << triangle->adjacent[2] << "]\n";
        }
        else {
            std::cout << "Triangle is null" << std::endl;
        }
    }
    static void printPoint(const DPoint* point) {
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
    static bool hasErrors(const DTriangle* item) {
        const auto& p = item->points;
        bool samePoints =
            (p[0]->id == p[1]->id) ||
            (p[1]->id == p[2]->id) ||
            (p[2]->id == p[0]->id);
        if (samePoints) {
            return true;
        }

        const auto& a = item->adjacent;
        bool sameAdjacents =
            (a[0] && a[0] == a[1]) ||
            (a[0] && a[0] == a[2]) ||
            (a[1] && a[1] == a[2]);
        if (sameAdjacents) {
            return true;
        }

        return false;
    }
    static int nearestIndex(const std::vector<DPoint*>& points, const glm::vec2& cursor, float& resultDistance) {
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

void SceneView::getUpdates(const Triangulation& model) {
        
    //update points 
    pointMeshes.resize(model.points.size());
    for (auto point : model.changedPoints) {
        pointMeshes[point->id] = CircleMesh::createPoint(point->position);
    }

    //update triangles
    triangleMeshes.resize(3 * model.triangles.size());
    for (auto triangle : model.changedTriangles) {
        const auto& p0 = triangle->points[0]->position;
        const auto& p1 = triangle->points[1]->position;
        const auto& p2 = triangle->points[2]->position;
            
        size_t idx = static_cast<size_t>(triangle->id * 3);
        triangleMeshes[idx + 0].movePosition(p0, p1);
        triangleMeshes[idx + 1].movePosition(p1, p2);
        triangleMeshes[idx + 2].movePosition(p2, p0);

        bool visible = !model.isSuper(triangle);
        triangleMeshes[idx + 0].visible = visible;
        triangleMeshes[idx + 1].visible = visible;
        triangleMeshes[idx + 2].visible = visible;
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

    triangulation.triangles.reserve(10);
    triangulation.points.reserve(8);
    triangulation.setWorkBox(-10, -10, w + 10, h + 10);

    triangulation.addPoint(left, bottom);
    triangulation.addPoint(right, bottom);
    triangulation.addPoint(right, top);
    triangulation.addPoint(left, top);
    triangulation.updateView(view);

    background.init(viewSize.x, viewSize.y);
    selectedPoint.mesh = CircleMesh::createPointSelected({ glm::vec2(0,0) });
}
void Scene::destroy() {
    background.destroy();
}
void Scene::addPoint(const glm::vec2& cursor) {
    triangulation.addPoint(cursor.x, cursor.y);
    triangulation.updateView(view);
}
void Scene::movePoint(const glm::vec2& cursor) {
    
    //skip unselected
    if (selectedPoint.index == -1) {
        return;
    }

    //skip super points
    auto point = triangulation.points[selectedPoint.index];
    if (triangulation.isSuper(point)) {
        return;
    }

    auto newPosition = cursor - dragOffset;
    triangulation.movePoint(selectedPoint.index, newPosition);
    triangulation.updateView(view);

    auto position = triangulation.points[selectedPoint.index]->position;
    selectedPoint.mesh.setPosition(position);
}
void Scene::selectPoint(const glm::vec2& cursor) {

    float distance = 0.f;
    auto index = util::nearestIndex(triangulation.points, cursor, distance);
    if (distance >= 20.f) {
        selectedPoint.index = -1;

        //select triangle
        for (auto t : triangulation.triangles) {
            uint8_t hitCode = 0;
            if (t->contains(cursor.x, cursor.y, hitCode)) {
                std::cout << "Selected triangle id=" << t->id << std::endl;
            }
        }

        return;
    }

    auto position = triangulation.points[index]->position;
    selectedPoint.mesh.setPosition(position);
    selectedPoint.index = index;
    dragOffset = cursor - position;
}
void Scene::recover() {
    selectedPoint.index = -1;

    if (triangulation.hasErrors()) {
        triangulation.rebuild();
        triangulation.updateView(view);
    }
}
void Scene::rebuild() {
    triangulation.rebuild();
    triangulation.updateView(view);
}


#include <iostream>
#include "src/platform/log.h"
#include "src/model/color.h"
#include "scene.h"


Circle::Circle() : radius(0) { }
Circle::Circle(float x, float y, float radius) : center(x,y), radius(radius) { }
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


EdgeIndex::EdgeIndex(): v0(0), v1(0) { }
EdgeIndex::EdgeIndex(unsigned int v0, unsigned int v1) : v0(std::min(v0, v1)), v1(std::max(v0, v1)) {}


const CircleMesh *Scene::getSelectedCircle() {
    if (selectedTriangle == -1) {
        return nullptr;
    }

    return nullptr;//&selectedCircleMesh;
}

void Scene::addPoint(const glm::vec2& cursor) {
    int lastIndex = static_cast<int>(points.size());
    points.emplace_back(lastIndex, cursor);

    auto mesh = createPointMesh(points.back().position);
    pointsMesh.emplace_back(mesh);

    triangulate();
    updateView();
}
void Scene::movePoint(const glm::vec2& cursor) {
    if (selectedPoint == -1) {
        return;
    }

    auto position = cursor - dragDrop;
    points[selectedPoint].position = position;

    updateView();
}
void Scene::deletePoint(const glm::vec2 &cursor) {

}
void Scene::selectPoint(const glm::vec2& cursor) {

    if (points.empty()) {
        return;
    }

    auto minDistance = glm::distance(points[0].position, cursor);
    dragDrop = cursor - points[0].position;
    auto index = 0;

    for(int i = 1; i < points.size(); i++) {
        auto& point = points[i];
        float distance = glm::distance(point.position, cursor);
        if (distance < minDistance) {
            minDistance = distance;
            index = i;
            dragDrop = cursor - points[i].position;
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
}
void Scene::clearSelection() {
    if (selectedPoint != -1) {
        pointsMesh[selectedPoint].color = Color::teal;
    }
    selectedPoint = -1;
    dragDrop.x = dragDrop.y = 0;
}

void Scene::initView(const glm::vec2& viewSize) {
    auto padding = 100.f;
    int lastIndex = static_cast<int>(points.size());
    points.emplace_back(lastIndex + 0, padding, padding);
    points.emplace_back(lastIndex + 1, static_cast<float>(viewSize.x) - padding, padding);
    points.emplace_back(lastIndex + 2, padding, static_cast<float>(viewSize.y) - padding);
    pointsMesh.reserve(points.size());
    for(auto& point : points) {
        auto mesh = createPointMesh(point.position);
        pointsMesh.push_back(mesh);
    }

    triangulate();
    updateView();
}
void Scene::updateView() {
    pointsMesh.resize(points.size());
    for(size_t i = 0; i < points.size(); i++) {
        auto& point = points[i];
        pointsMesh[i].setPosition(point.position);
    }

    trianglesMesh.resize(3 * triangles.size());
    for(auto i = 0; i < triangles.size(); i++) {
        auto& t = triangles[i];
        trianglesMesh[3*i+0] = createLineMesh(t.point[0].position, t.point[1].position);
        trianglesMesh[3*i+1] = createLineMesh(t.point[1].position, t.point[2].position);
        trianglesMesh[3*i+2] = createLineMesh(t.point[2].position, t.point[0].position);
    }
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

void Scene::normalizePoints() {
    if (points.empty()) {
        return;
    }

    auto min = points[0].position;
    auto max = points[0].position;
    for(auto& p : points) {
        min.x = std::min(min.x, p.position.x);
        min.y = std::min(min.y, p.position.y);
        max.x = std::max(max.x, p.position.x);
        max.y = std::max(max.y, p.position.y);
    }

    scale = std::max(max.x - min.x, max.y - min.y);
    if (scale == 0) {
        scale = std::max(max.x, max.y);
    }
    offset = min;


    for(auto& p : points) {
        p.position = (p.position - offset) / scale;
    }
}
void Scene::restorePoints() {
    if (points.empty()) {
        return;
    }

    for(auto& p : points) {
        p.position = p.position * scale + offset;
    }

    for(auto& t : triangles) {
        t.point[0] = points[t.point[0].index];
        t.point[1] = points[t.point[1].index];
        t.point[2] = points[t.point[2].index];
    }
}
void Scene::addSuperTriangle() {
    /* All existing points in [0,1] */
    points.reserve(points.size() + 3);

    int pointIndex = static_cast<int>(points.size());
    points.emplace_back(pointIndex + 0, -50, -50);
    points.emplace_back(pointIndex + 1, 50, -50);
    points.emplace_back(pointIndex + 2, 0, 50);

    triangles.emplace_back(
        static_cast<int>(triangles.size()),
        points[pointIndex],
        points[pointIndex + 1],
        points[pointIndex + 2]
    );
}
void Scene::removeSuperTriangle() {
    int pointIndex0 = points.size() - 3;
    int pointIndex1 = points.size() - 2;
    int pointIndex2 = points.size() - 1;

    std::vector<unsigned int> trianglesForDelete;
    for(size_t i = 0; i < triangles.size(); i++) {
        auto& t = triangles[i];
        if (t.has(pointIndex0) || t.has(pointIndex1) || t.has(pointIndex2)){
            trianglesForDelete.push_back(i);
        }
    }
    std::reverse(trianglesForDelete.begin(), trianglesForDelete.end());

    for(auto index : trianglesForDelete) {
        triangles.erase(triangles.begin() + index);
    }

    points.erase(points.begin() + pointIndex2);
    points.erase(points.begin() + pointIndex1);
    points.erase(points.begin() + pointIndex0);
}
void Scene::triangulate() {

    normalizePoints();
    addSuperTriangle();

    /* All points except super triangle */
    for(auto pointIndex = 0; pointIndex + 3 < points.size(); pointIndex++) {
        addPointToTriangulation(pointIndex);
    }

    //removeSuperTriangle();
    restorePoints();
}
void Scene::addPointToTriangulation(int pointIndex) {
    auto& point = points[pointIndex];
    auto triangleIndexForSplit = findTriangle(point.position);
    if (triangleIndexForSplit == -1) {
        Log::warn("Triangle not found!");
        return;
    }

    auto triangleForSplit = triangles[triangleIndexForSplit];
    auto p0 = triangleForSplit.point[0];
    auto p1 = triangleForSplit.point[1];
    auto p2 = triangleForSplit.point[2];

    int tIndex0 = triangleForSplit.index;
    int tIndex1 = static_cast<int>(triangles.size());
    int tIndex2 = tIndex1 + 1;
    triangles.resize(triangles.size() + 2);
    triangles[tIndex0] = Triangle {tIndex0, p0, p1, point};
    triangles[tIndex1] = Triangle {tIndex1, p1, p2, point};
    triangles[tIndex2] = Triangle {tIndex2, p2, p0, point};

    auto& t0 = triangles[tIndex0];
    auto& t1 = triangles[tIndex1];
    auto& t2 = triangles[tIndex2];

    t0.adjacent[0] = triangleForSplit.adjacent[0];
    t0.adjacent[1] = t1.index;
    t0.adjacent[2] = t2.index;

    t1.adjacent[0] = triangleForSplit.adjacent[1];
    t1.adjacent[1] = t2.index;
    t1.adjacent[2] = t0.index;

    t2.adjacent[0] = triangleForSplit.adjacent[2];
    t2.adjacent[1] = t0.index;
    t2.adjacent[2] = t1.index;

    for(auto i : triangleForSplit.adjacent) {
        if (i == -1) {
            continue;
        }

        auto& t = triangles[i];
        t.link(t0);
        t.link(t1);
        t.link(t2);
    }

//    struct TrianglePair {
//        int centerPoint;
//        int splittedTriangle;
//        int oppositeTriangle;
//    };
//
//    std::stack<TrianglePair> adjacent;
//    adjacent.push({pointIndex, t0.index, t0.getOpposite(pointIndex)});
//    adjacent.push({pointIndex, t1.index, t1.getOpposite(pointIndex)});
//    adjacent.push({pointIndex, t2.index, t2.getOpposite(pointIndex)});
}
int Scene::findTriangle(const glm::vec2& point) {
    for (size_t i = 0; i < triangles.size(); i++) {
        auto& triangle = triangles[i];
        if (triangle.contains(point)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}



//
//void Scene::selectTriangle(const glm::vec2& cursor) {
//    selectedTriangle = findTriangle(cursor);
//
//    if (selectedTriangle != -1) {
//        auto& circle = circles[selectedTriangle];
//        selectedCircle = createCircleMesh(circle);
//    }
//}
//void Scene::triangulate(const glm::vec2 &point) {
//    auto normalizedPoint = (point - offset) / scale;
//    int triangleIndex0 = findTriangle(normalizedPoint);
//    if (triangleIndex0 == -1) {
//        return;
//    }
//
//    auto triangleForSplit = triangles[triangleIndex0];
//
//    //vertex
//    auto v0 = triangleForSplit.v0;
//    auto v1 = triangleForSplit.v1;
//    auto v2 = triangleForSplit.v2;
//    auto v3 = static_cast<int>(points.size() - 1);
//    removeTriangleFromIndex(triangleIndex0);
//
//    //triangle
//    auto t0 = Triangle {v0, v1, v3};
//    auto t1 = Triangle {v1, v2, v3};
//    auto t2 = Triangle {v2, v0, v3};
//    triangles.reserve(triangles.size() + 2);
//    triangles[triangleIndex0] = t0;
//    triangles.push_back(t1);
//    triangles.push_back(t2);
//
//    //triangleIndex
//    int triangleIndex1 = static_cast<int>(triangles.size()-2);
//    int triangleIndex2 = static_cast<int>(triangles.size()-1);
//    addTriangleToIndex(triangleIndex0);
//    addTriangleToIndex(triangleIndex1);
//    addTriangleToIndex(triangleIndex2);
//}

//void Scene::createCircles() {
//    circles.resize(triangles.size());
//    for (size_t i = 0; i < triangles.size(); i++) {
//        auto& triangle = triangles[i];
//        circles[i] = createCircle(triangle);
//    }
//}
//Scene::Circle Scene::createCircle(const Triangle& triangle) {
//    auto& point1 = points[triangle.v0];
//    auto& point2 = points[triangle.v1];
//    auto& p3 = points[triangle.v2];
//    return Circle {point1, point2, p3};
//}






#include <iostream>
#include "scene.h"
#include "../model/color.h"

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

bool Triangle::contains(const glm::vec2& point) const {

    const float eps = 0.00000001f;
    auto pt = glm::vec3(point, 0);
    auto dir = glm::vec3(0, 0, 1);
    auto t0 = glm::vec3(p0, 0);
    auto t1 = glm::vec3(p1, 0);
    auto t2 = glm::vec3(p2, 0);

    if (glm::dot(glm::cross(t1 - t0, pt - t0), dir) < -eps) return false;
    if (glm::dot(glm::cross(pt - t0, t2 - t0), dir) < -eps) return false;
    if (glm::dot(glm::cross(t1 - pt, t2 - pt), dir) < -eps) return false;

    return true;
}


const CircleMesh *Scene::getSelectedCircle() {
    if (selectedTriangle == -1) {
        return nullptr;
    }

    return nullptr;//&selectedCircleMesh;
}

void Scene::addPoint(const glm::vec2& cursor) {
    points.emplace_back(cursor);

    auto mesh = createPointMesh(points.back());
    pointsMesh.emplace_back(mesh);

    normalizePoints();
    restorePoints();
    updateView();
}
void Scene::movePoint(const glm::vec2& cursor) {
    if (selectedPoint == -1) {
        return;
    }

    auto position = cursor - dragDrop;
    points[selectedPoint] = position;

    //todo: fix this
    triangles[0].p0 = points[0];
    triangles[0].p1 = points[1];
    triangles[0].p2 = points[2];

    updateView();
}
void Scene::deletePoint(const glm::vec2 &cursor) {

}
void Scene::selectPoint(const glm::vec2& cursor) {

    if (points.empty()) {
        return;
    }

    auto minDistance = glm::distance(points[0], cursor);
    dragDrop = cursor - points[0];
    auto index = 0;

    for(int i = 1; i < points.size(); i++) {
        auto& point = points[i];
        float distance = glm::distance(point, cursor);
        if (distance < minDistance) {
            minDistance = distance;
            index = i;
            dragDrop = cursor - points[i];
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

//    int triangleIndex = findTriangle(cursor);
//    std::cout << "triangleIndex = " << triangleIndex << std::endl;
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
    points.emplace_back(padding, padding);
    points.emplace_back(static_cast<float>(viewSize.x) - padding, padding);
    points.emplace_back(padding, static_cast<float>(viewSize.y) - padding);
    pointsMesh.reserve(points.size());
    for(auto& p : points) {
        auto mesh = createPointMesh(p);
        pointsMesh.push_back(mesh);
    }

    triangles.push_back(Triangle {points[0], points[1], points[2]});
    updateView();
}
void Scene::updateView() {
    pointsMesh.resize(points.size());
    for(size_t i = 0; i < points.size(); i++) {
        auto& point = points[i];
        pointsMesh[i].setPosition(point);
    }

    trianglesMesh.resize(3 * triangles.size());
    for(auto i = 0; i < triangles.size(); i++) {
        auto& t = triangles[i];
        trianglesMesh[3*i+0] = createLineMesh(t.p0, t.p1);
        trianglesMesh[3*i+1] = createLineMesh(t.p1, t.p2);
        trianglesMesh[3*i+2] = createLineMesh(t.p2, t.p0);
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

    auto min = points[0];
    auto max = points[0];
    for(auto& p : points) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
    }

    scale = std::max(max.x - min.x, max.y - min.y);
    if (scale == 0) {
        scale = std::max(max.x, max.y);
    }
    offset = min;


    for(auto& p : points) {
        p = (p - offset) / scale;
    }
}
void Scene::restorePoints() {
    if (points.empty()) {
        return;
    }

    for(auto& p : points) {
        p = p * scale + offset;
    }
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
//int Scene::findTriangle(const glm::vec2 &point) {
//    for (size_t i = 0; i < triangles.size(); i++) {
//
//        auto& t = triangles[i];
//        auto& p1 = points[t.v0];
//        auto& p2 = points[t.v1];
//        auto& p3 = points[t.v2];
//
//        bool contains = Triangle::contains(point, p1, p2, p3);
//        if (contains) {
//            return static_cast<int>(i);
//        }
//    }
//
//    return -1;
//}
//void Scene::createCircles() {
//    circles.resize(triangles.size());
//    for (size_t i = 0; i < triangles.size(); i++) {
//        auto& triangle = triangles[i];
//        circles[i] = createCircle(triangle);
//    }
//}
//Scene::Circle Scene::createCircle(const Triangle& triangle) {
//    auto& p1 = points[triangle.v0];
//    auto& p2 = points[triangle.v1];
//    auto& p3 = points[triangle.v2];
//    return Circle {p1, p2, p3};
//}












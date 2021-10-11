#include <iostream>
#include "scene.h"
#include "../model/color.h"


Scene::Circle::Circle() : radius(0) { }
Scene::Circle::Circle(float x, float y, float radius) : center(x,y), radius(radius) { }
Scene::Circle::Circle(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
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
bool Scene::Circle::contains(float x, float y) const {
    float dx = center.x - x;
    float dy = center.y - y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < radius;
}

Scene::Triangle::Triangle() : v0(0), v1(0), v2(0) { }
Scene::Triangle::Triangle(int v0, int v1, int v2) : v0(v0), v1(v1), v2(v2) { }
bool Scene::Triangle::contains(const glm::vec2& pt, const glm::vec2& t0, const glm::vec2& t1, const glm::vec2& t2) {
    return contains(
        glm::vec3(pt, 0),
        glm::vec3(0, 0, 1),
        glm::vec3(t0, 0),
        glm::vec3(t1, 0),
        glm::vec3(t2, 0)
    );
}
bool Scene::Triangle::contains(const glm::vec3& pt, const glm::vec3& dir, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2) {
    const float eps = 0.00000001f;

    if (glm::dot(glm::cross(t1 - t0, pt - t0), dir) < -eps) return false;
    if (glm::dot(glm::cross(pt - t0, t2 - t0), dir) < -eps) return false;
    if (glm::dot(glm::cross(t1 - pt, t2 - pt), dir) < -eps) return false;

    return true;
}

const CircleMesh *Scene::getSelectedCircle() {
    if (selectedTriangle == -1) {
        return nullptr;
    }

    return &selectedCircle;
}

void Scene::addPoint(const glm::vec2& cursor) {
    points.emplace_back(cursor);

    if (points.size() == 3) {

        Triangle triangle {0, 1, 2};
        triangles.push_back(triangle);

        Circle circle = createCircle(triangle);
        circles.push_back(circle);

        edges.push_back({0, 1});
        edges.push_back({1, 2});
        edges.push_back({2, 0});

        pointToTriangle[0].push_back(0);
        pointToTriangle[1].push_back(0);
        pointToTriangle[2].push_back(0);
    }

    if (points.size() > 3) {
        triangulate(cursor);
    }

    updateView();
}
void Scene::movePoint(const glm::vec2& cursor) {
    if (selectedPoint == -1) {
        return;
    }

    auto offset = cursor - dragDropDelta;
    auto& point = points[selectedPoint];
    point = offset;

    auto& vertex = pointsMesh[selectedPoint].vertex;
    for(auto& v : vertex) {
        v.move(point.x, point.y);
    }

    for(size_t i = 0; i < edges.size(); i++) {
        auto& edge = edges[i];
        auto& start = points[edge.v0];
        auto& end = points[edge.v1];
        edgesMesh[i].move(start, end);
    }

    for (size_t i = 0; i < triangles.size(); i++) {
        auto& triangle = triangles[i];
        auto& p1 = points[triangle.v0];
        auto& p2 = points[triangle.v1];
        auto& p3 = points[triangle.v2];
        circles[i] = Circle(p1, p2, p3);

        auto& circle = circles[i];
        circlesMesh[i].move(circle.center.x, circle.center.y, circle.radius);
    }
}
void Scene::selectPoint(const glm::vec2& cursor) {

    if (points.empty()) {
        return;
    }

    auto minDistance = glm::distance(points[0], cursor);
    dragDropDelta = cursor - points[0];
    auto index = 0;

    for(int i = 1; i < points.size(); i++) {
        auto& point = points[i];
        float distance = glm::distance(point, cursor);
        if (distance < minDistance) {
            minDistance = distance;
            index = i;
            dragDropDelta = cursor - points[i];
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

    int triangleIndex = findTriangle(cursor);
    std::cout << "triangleIndex = " << triangleIndex << std::endl;
}
void Scene::selectTriangle(const glm::vec2& cursor) {
    selectedTriangle = findTriangle(cursor);

    if (selectedTriangle != -1) {
        auto& circle = circles[selectedTriangle];
        selectedCircle = createCircleMesh(circle);
    }
}
void Scene::clearSelection() {
    if (selectedPoint != -1) {
        pointsMesh[selectedPoint].color = Color::teal;
    }
    selectedPoint = -1;
    dragDropDelta.x = dragDropDelta.y = 0;
}
void Scene::triangulate(const glm::vec2 &point) {
    int triangleIndex0 = findTriangle(point);
    if (triangleIndex0 == -1) {
        return;
    }

    auto triangleForSplit = triangles[triangleIndex0];

    //vertex
    auto v0 = triangleForSplit.v0;
    auto v1 = triangleForSplit.v1;
    auto v2 = triangleForSplit.v2;
    auto v3 = static_cast<int>(points.size() - 1);
    removeTriangleFromIndex(triangleIndex0);

    //triangle
    auto t0 = Triangle {v0, v1, v3};
    auto t1 = Triangle {v1, v2, v3};
    auto t2 = Triangle {v2, v0, v3};
    triangles.reserve(triangles.size() + 2);
    triangles[triangleIndex0] = t0;
    triangles.push_back(t1);
    triangles.push_back(t2);

    //triangleIndex
    int triangleIndex1 = static_cast<int>(triangles.size()-2);
    int triangleIndex2 = static_cast<int>(triangles.size()-1);
    addTriangleToIndex(triangleIndex0);
    addTriangleToIndex(triangleIndex1);
    addTriangleToIndex(triangleIndex2);

    //circle
    auto c0 = createCircle(t0);
    auto c1 = createCircle(t1);
    auto c2 = createCircle(t2);
    circles.reserve(triangles.size());
    circles[triangleIndex0] = c0;
    circles.push_back(c1);
    circles.push_back(c2);

    //edge
    auto e0 = Edge {v0, v3};
    auto e1 = Edge {v1, v3};
    auto e2 = Edge {v2, v3};
    edges.reserve(edges.size() + 3);
    edges.push_back(e0);
    edges.push_back(e1);
    edges.push_back(e2);
}
void Scene::addTriangleToIndex(int index) {
    auto& triangle = triangles[index];
    pointToTriangle[triangle.v0].push_back(index);
    pointToTriangle[triangle.v1].push_back(index);
    pointToTriangle[triangle.v2].push_back(index);
}
void Scene::removeTriangleFromIndex(int index) {
    auto& triangle = triangles[index];
    std::erase_if(pointToTriangle[triangle.v0], [index](int t) { return t == index; });
    std::erase_if(pointToTriangle[triangle.v1], [index](int t) { return t == index; });
    std::erase_if(pointToTriangle[triangle.v2], [index](int t) { return t == index; });
}

int Scene::findTriangle(const glm::vec2 &point) {
    for (size_t i = 0; i < triangles.size(); i++) {

        auto& t = triangles[i];
        auto& p1 = points[t.v0];
        auto& p2 = points[t.v1];
        auto& p3 = points[t.v2];

        bool contains = Triangle::contains(point, p1, p2, p3);
        if (contains) {
            return static_cast<int>(i);
        }
    }

    return -1;
}
Scene::Circle Scene::createCircle(const Triangle& triangle) {
    auto& p1 = points[triangle.v0];
    auto& p2 = points[triangle.v1];
    auto& p3 = points[triangle.v2];
    return Circle {p1, p2, p3};
}

void Scene::updateView() {
    pointsMesh.resize(points.size());
    for(size_t i = 0; i < points.size(); i++) {
        auto& point = points[i];
        pointsMesh[i] = createPointMesh(point);
    }

    edgesMesh.resize(edges.size());
    for(size_t i = 0; i < edges.size(); i++) {
        auto& edge = edges[i];
        edgesMesh[i] = createLineMesh(edge);
    }

    circlesMesh.resize(circles.size());
    for(size_t i = 0; i < circles.size(); i++) {
        auto& circle = circles[i];
        circlesMesh[i] = createCircleMesh(circle);
    }
}
LineMesh Scene::createLineMesh(const Scene::Edge &edge) {
    auto& start = points[edge.v0];
    auto& end = points[edge.v1];
    return LineMesh::create(start, end, Color::orange, 3.5f);
}
CircleMesh Scene::createPointMesh(const glm::vec2& point) {
    return CircleMesh(point, 7.f, true, Color::teal);
}
CircleMesh Scene::createCircleMesh(const Circle& circle) {
    return CircleMesh {circle.center, circle.radius, false, Color::teal };
}




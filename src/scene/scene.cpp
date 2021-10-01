#include <iostream>
#include "scene.h"
#include "../model/color.h"

bool Scene::Circle::contains(float x, float y) const {
    float dx = center.x -  x;
    float dy = center.y - y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < radius;
}

Scene::Circle::Circle() : radius(0) { }
Scene::Circle::Circle(float x, float y, float radius) : center(x,y), radius(radius) { }
Scene::Circle Scene::Circle::get(float x1, float y1, float x2, float y2,float x3, float y3) {
    float m1 = (x1 * x1 + y1 * y1);
    float m2 = (x2 * x2 + y2 * y2);
    float m3 = (x3 * x3 + y3 * y3);

    float A = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
    float B = m1 * (y3 - y2) + m2 * (y1 - y3) + m3 * (y2 - y1);
    float C = m1 * (x2 - x3) + m2 * (x3 - x1) + m3 * (x1 - x2);
    float D = m1 * (x3 * y2 - x2 * y3) + m2 * (x1 * y3 - x3 * y1) + m3 * (x2 * y1 - x1 * y2);

    float x = -B / (2 * A);
    float y = -C / (2 * A);
    float r = sqrt((B * B + C * C - 4 * A * D) / (4 * A * A));

    return Circle(x,y,r);
}

void Scene::addPoint(float x, float y) {
    points.emplace_back(x, y);

    auto circleMesh = CircleMesh(glm::vec2(x, y), 10.f, true, Color::teal);
    pointsMesh.push_back(circleMesh);

    if (points.size() == 3) {

        auto t = createTriangle(0, 1, 2);
        triangles.push_back(t);

        edges.push_back({0, 1});
        edges.push_back({1, 2});
        edges.push_back({2, 0});

        edgesMesh.reserve(edges.size());
        for(auto& e : edges) {
            auto& start = points[e.v0];
            auto& end = points[e.v1];
            auto mesh = LineMesh::create(start, end, Color::orange, 3.5f);
            edgesMesh.push_back(mesh);
        }
    }

    if (points.size() > 3) {
        triangulate();
    }
}
void Scene::selectPoint(float x, float y) {
    if (points.empty()) {
        return;
    }

    auto cursor = glm::vec2(x, y);
    auto minDistance = glm::distance(points[0], cursor);
    auto index = 0;

    for(int i = 1; i < points.size(); i++) {
        auto& point = points[i];
        float distance = glm::distance(point, cursor);
        if (distance < minDistance) {
            minDistance = distance;
            index = i;
        }
    }

    if (selectedIndex != -1) {
        pointsMesh[selectedIndex].color = Color::teal;
    }

    if (minDistance >= 20.f) {
        selectedIndex = -1;
        return;
    }

    selectedIndex = index;
    pointsMesh[selectedIndex].color = Color::yellow;
    std::cout << selectedIndex << std::endl;
}
void Scene::movePoint(float x, float y) {
    if (selectedIndex == -1) {
        return;
    }

    auto& point = points[selectedIndex];
    point.x = x;
    point.y = y;

    auto& vertex = pointsMesh[selectedIndex].vertex;
    for(auto& v : vertex) {
        v.move(x, y);
    }

    for(size_t i = 0; i < edges.size(); i++) {
        auto& edge = edges[i];
        auto& start = points[edge.v0];
        auto& end = points[edge.v1];
        edgesMesh[i].move(start, end);
    }
}
void Scene::clearSelection() {
    if (selectedIndex != -1) {
        pointsMesh[selectedIndex].color = Color::teal;
    }
    selectedIndex = -1;
}
void Scene::triangulate() {
    int triangleIndex = 0; //findTriangleIndex();
    auto triangleForSplit = triangles[triangleIndex];

    auto v0 = triangleForSplit.v0;
    auto v1 = triangleForSplit.v1;
    auto v2 = triangleForSplit.v2;
    auto v3 = static_cast<int>(points.size() - 1);

    auto t0 = createTriangle(v0, v1, v3);
    auto t1 = createTriangle(v1, v2, v3);
    auto t2 = createTriangle(v2, v0, v3);

    triangles.reserve(triangles.size() + 2);
    triangles[triangleIndex] = t0;
    triangles.push_back(t1);
    triangles.push_back(t2);


    auto e0 = Edge {v0, v3};
    auto e1 = Edge {v1, v3};
    auto e2 = Edge {v2, v3};
}

Scene::Triangle Scene::createTriangle(int v0, int v1, int v2) {
    auto& p1 = points[v0];
    auto& p2 = points[v1];
    auto& p3 = points[v2];

    Triangle triangle {v0, v1, v2};
    triangle.circle = Circle::get(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);

    return triangle;
}



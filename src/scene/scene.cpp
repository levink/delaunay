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


const CircleMesh *Scene::getSelectedCircle() {
    if (selectedTriangle == -1) {
        return nullptr;
    }

    return nullptr;//&selectedCircleMesh;
}

void Scene::initScene(const glm::vec2& viewSize) {
    auto padding = 100.f;
    auto left = padding;
    auto right = static_cast<float>(viewSize.x) - padding;
    auto top = static_cast<float>(viewSize.y) - padding;
    auto bottom = padding;

    points.clear();
    
    //super triangle points
    points.emplace_back(0, -10000, -10000);
    points.emplace_back(1, 10000, -10000);
    points.emplace_back(2, 0, 10000);

    //visible points
    points.emplace_back(3, left, bottom);
    points.emplace_back(4, right, bottom);
    points.emplace_back(5, right, top);
    points.emplace_back(6, left, top);

    pointsMesh.reserve(points.size());
    for(auto& point : points) {
        auto mesh = createPointMesh(point.getPosition());
        pointsMesh.push_back(mesh);
    }

    triangulate();
    updateView();

    addPoint({375, 25});
}
void Scene::updateView() {
    pointsMesh.resize(points.size());
    for(size_t i = 0; i < points.size(); i++) {
        auto& point = points[i];
        pointsMesh[i].setPosition(point.getPosition());
    }

    auto pIndex0 = points[0].index;
    auto pIndex1 = points[1].index;
    auto pIndex2 = points[2].index;
    
    trianglesMesh.reserve(3 * triangles.size());
    trianglesMesh.clear();
    
    for (auto& triangle : triangles) {

        bool superTriangle =
            triangle.has(pIndex0) ||
            triangle.has(pIndex1) ||
            triangle.has(pIndex2);
        if (superTriangle) {
            continue;
        }

        auto p0 = triangle.point[0].getPosition();
        auto p1 = triangle.point[1].getPosition();
        auto p2 = triangle.point[2].getPosition();

        trianglesMesh.push_back(createLineMesh(p0, p1));
        trianglesMesh.push_back(createLineMesh(p1, p2));
        trianglesMesh.push_back(createLineMesh(p2, p0));
    }
}
void Scene::addPoint(const glm::vec2& cursor) {
    int lastIndex = static_cast<int>(points.size());
    points.emplace_back(lastIndex, cursor);

    std::cout << "Point " << lastIndex << " " << cursor.x << ", " << cursor.y << std::endl;

    auto pos = points.back().getPosition();
    auto mesh = createPointMesh(pos);
    pointsMesh.emplace_back(mesh);

    triangulate();
    updateView();
}
void Scene::movePoint(const glm::vec2& cursor) {
    if (selectedPoint == -1) {
        return;
    }

    auto position = cursor - dragDrop;
    points[selectedPoint].setPosition(position);

    triangulate();
    updateView();
}
void Scene::deletePoint(const glm::vec2 &cursor) {

}
void Scene::selectPoint(const glm::vec2& cursor) {

    if (points.empty()) {
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
    Log::out(msg);
}
void Scene::clearSelection() {
    if (selectedPoint != -1) {
        pointsMesh[selectedPoint].color = Color::teal;
    }
    selectedPoint = -1;
    dragDrop.x = dragDrop.y = 0;
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
void Scene::triangulate() {

    normalizePoints();
    triangles.clear();
    
    triangles.emplace_back(
        static_cast<int>(triangles.size()),
        points[0],
        points[1],
        points[2]
    );

    /* All points except super triangle */
    for(auto pointIndex = 3; pointIndex < points.size(); pointIndex++) {
        addPointToTriangulation(pointIndex);
    }

    restorePoints();
}
void Scene::normalizePoints() {
    if (points.empty()) {
        return;
    }

    auto min = points[0].getPosition();
    auto max = points[0].getPosition();
    for(auto& p : points) {
        auto pos = p.getPosition();
        min.x = std::min(min.x, pos.x);
        min.y = std::min(min.y, pos.y);
        max.x = std::max(max.x, pos.x);
        max.y = std::max(max.y, pos.y);
    }

    scale = std::max(max.x - min.x, max.y - min.y);
    if (scale == 0) {
        scale = std::max(max.x, max.y);
    }
    offset = min;


    for(auto& p : points) {
        auto value = (p.getPosition() - offset) / scale;
        p.setPosition(value);
    }
}
void Scene::restorePoints() {
    if (points.empty()) {
        return;
    }

    for(auto& p : points) {
        auto value = p.getPosition() * scale + offset;
        p.setPosition(value);
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
    points.emplace_back(pointIndex + 0, -100, -100);
    points.emplace_back(pointIndex + 1, 100, -100);
    points.emplace_back(pointIndex + 2, 0, 100);

    triangles.emplace_back(
        static_cast<int>(triangles.size()),
        points[pointIndex],
        points[pointIndex + 1],
        points[pointIndex + 2]
    );
}
void Scene::removeSuperTriangle() {
    int size = static_cast<int>(points.size());
    int pointIndex0 = size - 3;
    int pointIndex1 = size - 2;
    int pointIndex2 = size - 1;

    std::vector<int> forDelete;
    for(int i = triangles.size() - 1; i >= 0; i--) {
        auto& t = triangles[i];
        if (t.has(pointIndex0) || t.has(pointIndex1) || t.has(pointIndex2)){
            forDelete.push_back(i);
        }
    }

    for(auto index : forDelete) {
        triangles.erase(triangles.begin() + index);
        for(auto& t : triangles) {
            t.replaceAdjacent(index, -1);
            if (t.adjacent[0] > index) {
                t.adjacent[0]--;
            }
            if (t.adjacent[1] > index) {
                t.adjacent[1]--;
            }
            if (t.adjacent[2] > index) {
                t.adjacent[2]--;
            }
            if (t.index > index) {
                t.index--;
            }
        }
    }

    points.erase(points.begin() + pointIndex2);
    points.erase(points.begin() + pointIndex1);
    points.erase(points.begin() + pointIndex0);
}
void Scene::addPointToTriangulation(int pointIndex) {
    auto& point = points[pointIndex];
    auto triangleIndexForSplit = findTriangle(point.position);
    if (triangleIndexForSplit == -1) {
        Log::warn("Triangle not found!");
        return;
    }

    auto triangleForSplit = triangles[triangleIndexForSplit];
    const auto& p0 = triangleForSplit.point[0];
    const auto& p1 = triangleForSplit.point[1];
    const auto& p2 = triangleForSplit.point[2];

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

    t0.checkError();
    t1.checkError();
    t2.checkError();

    //split triangle
    {
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

    }


    std::stack<int> checkItems;
    checkItems.push(t0.index);
    checkItems.push(t1.index);
    checkItems.push(t2.index);

    while(!checkItems.empty()) {
        auto splitIndex = checkItems.top();
        checkItems.pop();

        if (splitIndex == -1) {
            continue;
        }

        auto& splitted = triangles[splitIndex];
        auto oppositeIndex = splitted.getOpposite(pointIndex);
        if (oppositeIndex == -1) {
            continue;
        }

        auto& opposite = triangles[oppositeIndex];
        auto hull = splitted.getHull(opposite);
        auto concave = !hull.isConvex();
        if (concave) {
            continue;
        }
        auto position = point.getPosition();
        auto circle = opposite.getCircle();
        auto delaunay = !circle.contains(position);
        if (delaunay) {
            continue;
        }

        auto swap = swapEdge(point, splitted, opposite);
        if (swap.success) {
            checkItems.push(swap.first);
            checkItems.push(swap.second);
        }
    }
}

SwapResult Scene::swapEdge(const Point &splitPoint, Triangle &old1, Triangle &old2) {
    old1.checkError();
    old2.checkError();

    if (!old1.linkedWith(old2) || !old2.linkedWith(old1)) {
        Log::warn("[swapEdge] Triangles are not linked!");
        return SwapResult{false};
    }

    auto commonEdge = old1.getCommonEdge(old2);
    old1.setFirst(commonEdge.v0);
    old2.setFirst(commonEdge.v1);

    auto& p1 = points[commonEdge.v0];
    auto& p2 = points[commonEdge.v1];
    auto& p3 = points[old1.getOppositePoint(commonEdge)];
    auto& p4 = points[old2.getOppositePoint(commonEdge)];

    auto new1 = Triangle {old1.index, p1, p4, p3 };
    auto new2 = Triangle {old2.index, p2, p3, p4 };
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

    return SwapResult {
            true,
            new1.index,//, new1.adjacent[2]},
            new2.index //, new2.adjacent[0]}
    };
}
int Scene::findTriangle(float position[2]) {
    glm::vec2 pos = {position[0], position[1]};
    for (size_t i = 0; i < triangles.size(); i++) {
        auto& triangle = triangles[i];
        if (triangle.contains(pos)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}







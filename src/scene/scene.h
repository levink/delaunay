#pragma once

#include "pch.h"
#include "model/delaunay.h"
#include "model/drawBatch.h"
#include "view/line.h"
#include "view/circle.h"

struct SceneView : public delaunay::Triangulation::Observer {
    std::vector<CircleMesh> pointMeshes;
    std::vector<LineMesh> triangleMeshes;
    void getUpdates(const delaunay::Triangulation& model) override;
};

struct SelectedPoint {
    int index = -1;
    CircleMesh mesh;
};

struct Scene {
    DrawBatch background;
    delaunay::Triangulation triangulation;
    SceneView view;
    glm::vec2 dragOffset;
    SelectedPoint selectedPoint;

    void init(const glm::vec2& viewSize);
    void destroy();
    void addPoint(const glm::vec2& cursor);
    void movePoint(const glm::vec2& cursor);
    void selectPoint(const glm::vec2& cursor);
    void recover();
    void rebuild();
};


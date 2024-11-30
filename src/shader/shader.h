#pragma once
#include <vector>
#include "model/drawBatch.h"
#include "scene/camera.h"
#include "shader/shaderBase.h"
#include "view/circle.h"
#include "view/line.h"

class CircleShader : public Shader {
    struct {
        const Camera* camera = nullptr;
    } context;
    void drawMesh(const CircleMesh& mesh);
public:
    CircleShader();
    void link(const Camera* camera);
    void enable() const override;
    void disable() const override;
    void draw(const CircleMesh& mesh);
    void draw(const std::vector<CircleMesh>& meshes);
    void draw(const DrawBatch& batch, const glm::vec3& color);
};


class LineShader : public Shader {
    struct {
        const Camera* camera = nullptr;
    } context;
public:
    LineShader();
    void link(const Camera* camera);
    void enable() const override;
    void disable() const override;
    void draw(const std::vector<LineMesh>& items);
};
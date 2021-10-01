#pragma once
#include "src/pch.h"
#include "src/camera/camera.h"
#include "src/model/circle.h"
#include "src/model/line.h"
#include "src/scene/drawBatch.h"
#include "shaderBase.h"

class CircleShader : public Shader {
    struct {
        const Camera* camera = nullptr;
    } context;
public:
    explicit CircleShader();
    void link(const Camera* camera);
    void enable() const override;
    void disable() const override;
    void draw(const std::vector<CircleMesh>& items);
    void draw(const DrawBatch& batch, const glm::vec3& color);
};


class LineShader : public Shader {
    struct {
        const Camera* camera = nullptr;
    } context;
public:
    explicit LineShader();
    void link(const Camera* camera);
    void enable() const override;
    void disable() const override;
    void draw(const std::vector<LineMesh>& items);
};
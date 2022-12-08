#pragma once
#include "pch.h"
#include "camera/camera.h"
#include "model/circle.h"
#include "model/line.h"
#include "scene/drawBatch.h"
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
    void draw(const CircleMesh* mesh);
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
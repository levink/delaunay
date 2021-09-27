#pragma once
#include "src/pch.h"
#include "src/camera/camera.h"
#include "src/model/circle.h"
#include "src/model/line.h"
#include "shaderBase.h"
#include "drawBatch.h"

class CircleShader : public Shader {
    struct {
        const Camera* camera = nullptr;
    } context;
public:
    explicit CircleShader();
    void link(const Camera* camera);
    void enable() const override;
    void disable() const override;
    void draw(const std::vector<CircleModel>& items, const glm::vec3& color);
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

    void draw(const std::vector<LineVertex>& vertex, const glm::vec3& color);
};
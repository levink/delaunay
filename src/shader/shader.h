#pragma once
#include "src/pch.h"
#include "src/camera/camera.h"
#include "src/model/circle.h"
#include "shaderBase.h"


class CircleShader : public BaseShader {
    struct {
        const Camera* camera = nullptr;
    } context;
public:
    explicit CircleShader();
    void link(const Camera* camera);
    void draw(const std::vector<CircleVertex>& vertex, const std::vector<Face>& face);
    void enable() override;
    void disable() override;
};
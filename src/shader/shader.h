#pragma once
#include "src/pch.h"
#include "src/camera/camera.h"
#include "src/model/circle.h"
#include "src/model/line.h"
#include "shaderBase.h"


class CircleShader : public Shader {
    struct {
        const Camera* camera = nullptr;
    } context;
public:
    explicit CircleShader();
    void link(const Camera* camera);
    void enable() override;
    void draw(const std::vector<CircleVertex>& vertex, const std::vector<Face>& face);
    void disable() override;
};


class LineShader : public Shader {
    struct {
        const Camera* camera = nullptr;
    } context;
public:
    explicit LineShader();
    void link(const Camera* camera);
    void enable() override;
    void draw(const std::vector<LineVertex>& vertex);
    void disable() override;
};
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string_view>
#include "src/camera/camera.h"
#include "src/model/circle.h"
#include "shaderBase.h"


class CircleShader : public BaseShader {
    struct {
        const Camera* camera = nullptr;
    } context;
public:
    explicit CircleShader();
    void init() override;
    void link(const Camera* camera);
    void draw(const CircleModel& model);
};
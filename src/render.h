//
// Created by Konst on 22.09.2021.
//
#pragma once

#include <src/camera/camera.h>
#include <src/platform/loader.h>
#include <src/shader/shaderSource.h>
#include <src/shader/shader.h>

struct ShaderCache {
    bool loaded = false;
    ShaderSource circle;
};

struct RenderShaders {
    CircleShader circle;
};

class Render {
    ShaderCache shaderCache;
public:
    Camera camera;
    RenderShaders shader;
    std::vector<CircleModel> circles;

    void load(Platform& platform);
    void init();
    void destroy();
    void draw();
};
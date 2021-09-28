#pragma once
#include "camera/camera.h"
#include "platform/loader.h"
#include "shader/shaderSource.h"
#include "shader/shader.h"
#include "layer.h"

class Render;

struct ShaderCache {
    bool loaded = false;
    ShaderSource circle;
    ShaderSource line;
    void load(const ShaderLoader& loader);
};

struct Shaders {
    CircleShader circle;
    LineShader line;
    void create(const ShaderCache& cache);
    void link(const Render& render);
    void destroy();
};

class Render {
    ShaderCache shaderCache;
public:
    Shaders shaders;
    Camera camera;

    DrawBatch circlesBatch;
    Layer layer;

    void load(Platform& platform);
    void init();
    void destroy();
    void draw();
    void reshape(int width, int height);
    void reloadShaders(Platform& platform);
};
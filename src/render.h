#pragma once
#include <src/camera/camera.h>
#include <src/platform/loader.h>
#include <src/shader/shaderSource.h>
#include <src/shader/shader.h>

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
    Line line;

    std::vector<CircleModel> circles;

    void load(Platform& platform);
    void init();
    void destroy();
    void draw();
    void reshape(int width, int height);
    void reloadShaders(Platform& platform);

    void add(const CircleModel &model);
};
#pragma once
#include "camera/camera.h"
#include "platform/loader.h"
#include "shader/shaderSource.h"
#include "shader/shader.h"
#include "scene/scene.h"
using namespace delaunay;

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
    Scene scene;

    void loadResources();
    void initResources();
    void initScene();
    void draw();
    void destroy();
    void reshape(int width, int height);
    void reloadShaders();
};
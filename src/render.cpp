#include <random>
#include "model/color.h"
#include "render.h"


void ShaderCache::load(const ShaderLoader &loader) {
    circle = loader.load(files::circle);
    line = loader.load(files::line);
}
void Shaders::create(const ShaderCache &cache) {
    circle.create(cache.circle);
    line.create(cache.line);
}
void Shaders::link(const Render &render) {
    circle.link(&render.camera);
    line.link(&render.camera);
}
void Shaders::destroy() {
    circle.destroy();
    line.destroy();
}
void Render::loadResources(Platform &platform) {
    if (shaderCache.loaded) {
        return;
    }
    ShaderLoader loader(platform);
    shaderCache.load(loader);
}
void Render::initResources() {
    shaders.create(shaderCache);
    shaders.link(*this);
}
void Render::destroy() {
    scene.destroy();
    shaders.destroy();
}
void Render::initScene() {
    scene.init(camera.viewSize);
}
void Render::draw() {

    auto background = glm::vec4(Color::asphalt, 1.0);
    glClearColor(background.r,
                 background.g,
                 background.b,
                 background.a);
    glClear(GL_COLOR_BUFFER_BIT);

    shaders.circle.enable();
    shaders.circle.draw(scene.background, Color::black);
    shaders.circle.disable();

    shaders.line.enable();
    shaders.line.draw(scene.view.trianglesMesh);
    shaders.line.disable();

    shaders.circle.enable();
    shaders.circle.draw(scene.view.pointsMesh);
    if (scene.view.selectedPoint.active) {
        shaders.circle.draw(scene.view.selectedPoint.mesh);
    }
    shaders.circle.disable();
}
void Render::reshape(int w, int h) {
    camera.reshape(w, h);
}
void Render::reloadShaders(Platform &platform) {
    ShaderLoader loader(platform);
    shaderCache.load(loader);
    shaders.destroy();
    shaders.create(shaderCache);
    shaders.link(*this);
}

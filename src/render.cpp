#include <src/model/color.h>
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

void Render::load(Platform &platform) {
    if (shaderCache.loaded) {
        return;
    }
    ShaderLoader loader(platform);
    shaderCache.load(loader);
}
void Render::init() {
    shaders.create(shaderCache);
    shaders.link(*this);
}
void Render::destroy() {
    shaders.destroy();
    circlesBatch.destroy();
}
void Render::draw() {
    glClearColor(Color::asphalt.r,
                 Color::asphalt.g,
                 Color::asphalt.b,
                 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shaders.circle.enable();
    shaders.circle.draw(circlesBatch, Color::teal);
    shaders.circle.draw(circles, Color::orange);
    shaders.circle.disable();

    shaders.line.enable();
    shaders.line.draw(lineVertices, Color::orange);
    shaders.line.disable();
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
void Render::add(const CircleModel &model) {
    circles.push_back(model);
}



//
// Created by Konst on 22.09.2021.
//
#include <src/model/color.h>
#include "render.h"

void Render::load(Platform &platform) {
    if (shaderCache.loaded) {
        return;
    }

    ShaderLoader loader(platform);
    shaderCache.circle = loader.load(files::circle);
}
void Render::init() {
    circleShader.create(shaderCache.circle);
    circleShader.link(&camera);
}
void Render::destroy() {
    circleShader.destroy();
}
void Render::draw() {
    glClearColor(Color::asphalt.r,
                 Color::asphalt.g,
                 Color::asphalt.b,
                 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    circleShader.enable();
    circleShader.draw(circleVertices, circleFaces);
    circleShader.disable();
}
void Render::reshape(int w, int h) {
    camera.reshape(w, h);
}
void Render::reloadShaders(Platform &platform) {
    ShaderLoader loader(platform);
    shaderCache.circle = loader.load(files::circle);
    circleShader.create(shaderCache.circle);
    circleShader.link(&camera);
}

void Render::add(const CircleModel &model) {

    auto offset = circleVertices.size();
    circleVertices.insert(end(circleVertices), begin(model.vertices), end(model.vertices));

    for(auto& face : model.faces) {
        auto a = face.a + offset;
        auto b = face.b + offset;
        auto c = face.c + offset;
        circleFaces.emplace_back(a, b, c);
    }
}


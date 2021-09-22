//
// Created by Konst on 22.09.2021.
//
#include <src/model/color.h>
#include "render.h"

void Render::load(Platform &platform) {
    ShaderLoader loader(platform);
    shaderCache.circle = loader.load(files::circleVert, files::circleFrag);
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

    glm::vec2 center = camera.viewSize / 2;
    float r = 100;

    CircleModel model;
    model.vertex = {
            glm::vec2(center.x - r, center.y - r),
            glm::vec2(center.x - r, center.y + r),
            glm::vec2(center.x + r, center.y + r),
            glm::vec2(center.x + r, center.y - r),
    };
    model.faces = {
            {0, 1,2},
            {2, 3,0},
    };
    model.radius = r;
    model.center = center;
    model.color = &Color::teal;

    circleShader.enable();
    circleShader.draw(model);
    circleShader.disable();
}

void Render::reshape(int w, int h) {
    camera.reshape(w, h);
}
void Render::reloadShaders(Platform &platform) {
    ShaderLoader loader(platform);
    shaderCache.circle = loader.load(files::circleVert, files::circleFrag);
    circleShader.create(shaderCache.circle);
    circleShader.link(&camera);
}


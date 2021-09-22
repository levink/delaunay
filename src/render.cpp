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
    shader.circle.create(shaderCache.circle);
}
void Render::destroy() {
    shader.circle.destroy();
}
void Render::draw() {
    glClearColor(Color::asphalt.r,
                 Color::asphalt.g,
                 Color::asphalt.b,
                 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.circle.enable();
    shader.circle.disable();
}

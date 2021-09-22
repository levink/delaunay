#include <src/model/circle.h>
#include "shader.h"

CircleShader::CircleShader() : BaseShader(1, 4) { }
void CircleShader::init() {
    a[0] = attribute("in_Position");
    u[0] = uniform("Ortho");
    u[1] = uniform("Color");
    u[2] = uniform("Center");
    u[3] = uniform("Radius");
}
void CircleShader::link(const Camera* camera) {
    context.camera = camera;
}
void CircleShader::enable() {
    BaseShader::enable();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void CircleShader::draw(const std::vector<CircleModel>& items) {
    set4(u[0], context.camera->Ortho);
    for(auto& item : items) {
        set3(u[1], item.color);
        set2(u[2], item.center);
        set1(u[3], item.radius);
        attr(a[0], item.vertex, Attribute::vec2);
        glDrawElements(GL_TRIANGLES, (int)item.faces.size() * 3, GL_UNSIGNED_SHORT, item.faces.data());
    }
}
void CircleShader::disable() {
    BaseShader::disable();
    glDisable(GL_BLEND);
}


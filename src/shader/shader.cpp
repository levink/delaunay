#include <src/model/circle.h>
#include "shader.h"

CircleShader::CircleShader() : BaseShader(1, 4) { }
void CircleShader::init() {
    a[0] = attr("in_Position");
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
void CircleShader::draw(const CircleModel& model) {
    set4(u[0], context.camera->Ortho);
    set3(u[1], model.color);
    set2(u[2], model.center);
    set1(u[3], model.radius);
    setAttr(a[0], model.vertex, Attribute::vec2);
    glDrawElements(GL_TRIANGLES, (int)model.faces.size() * 3, GL_UNSIGNED_SHORT, model.faces.data());
}
void CircleShader::disable() {
    BaseShader::disable();
    glDisable(GL_BLEND);
}


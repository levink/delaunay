#include "shader.h"

CircleShader::CircleShader() : Shader(2, 4) {
    u[0] = Uniform("Ortho");
    u[1] = Uniform("Color");
    a[0] = Attribute(VEC_2, "in_Position");
    a[1] = Attribute(VEC_2 ,"in_Center");
    a[2] = Attribute(FLOAT, "in_Radius");
    a[3] = Attribute(FLOAT, "in_Fill");
}
void CircleShader::link(const Camera* camera) {
    context.camera = camera;
}
void CircleShader::enable() {
    Shader::enable();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void CircleShader::draw(const std::vector<CircleVertex>& vertex, const std::vector<Face>& face, const glm::vec3& color) {
    set4(u[0], context.camera->Ortho);
    set3(u[1], color);
    attr(a[0], vertex, sizeof(CircleVertex), offsetof(CircleVertex, position));
    attr(a[1], vertex, sizeof(CircleVertex), offsetof(CircleVertex, center));
    attr(a[2], vertex, sizeof(CircleVertex), offsetof(CircleVertex, radius));
    attr(a[3], vertex, sizeof(CircleVertex), offsetof(CircleVertex, fill));

    glDrawElements(GL_TRIANGLES, (int)face.size() * 3, GL_UNSIGNED_SHORT, face.data());
}
void CircleShader::disable() {
    Shader::disable();
    glDisable(GL_BLEND);
}


LineShader::LineShader() : Shader(2, 1) {
    u[0] = Uniform("Ortho");
    u[1] = Uniform("Color");
    a[0] = Attribute(VEC_2, "in_Position");
}
void LineShader::link(const Camera *camera) {
    context.camera = camera;
}
void LineShader::enable() {
    Shader::enable();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.f);
}
void LineShader::draw(const std::vector<LineVertex> &vertex, const glm::vec3& color) {
    set4(u[0], context.camera->Ortho);
    set3(u[1], color);
    attr(a[0], vertex, sizeof(LineVertex), offsetof(LineVertex, position));
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)vertex.size());
}
void LineShader::disable() {
    Shader::disable();
    glDisable(GL_BLEND);
    glLineWidth(1.f);
}


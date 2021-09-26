#include "shader.h"

CircleShader::CircleShader() : Shader(1, 5) {
    u[0] = Uniform("Ortho");
    a[0] = Attribute(VEC_2, "in_Position");
    a[1] = Attribute(VEC_3, "in_Color");
    a[2] = Attribute(VEC_2 ,"in_Center");
    a[3] = Attribute(FLOAT, "in_Radius");
    a[4] = Attribute(FLOAT, "in_Fill");
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
void CircleShader::draw(const std::vector<CircleVertex>& vertex, const std::vector<Face>& face) {
    set4(u[0], context.camera->Ortho);
    attr(a[0], vertex, sizeof(CircleVertex), offsetof(CircleVertex, position));
    attr(a[1], vertex, sizeof(CircleVertex), offsetof(CircleVertex, color));
    attr(a[2], vertex, sizeof(CircleVertex), offsetof(CircleVertex, center));
    attr(a[3], vertex, sizeof(CircleVertex), offsetof(CircleVertex, radius));
    attr(a[4], vertex, sizeof(CircleVertex), offsetof(CircleVertex, fill));

    glDrawElements(GL_TRIANGLES, (int)face.size() * 3, GL_UNSIGNED_SHORT, face.data());
}
void CircleShader::disable() {
    Shader::disable();
    glDisable(GL_BLEND);
}


LineShader::LineShader() : Shader(1, 1) {
    u[0] = Uniform("Ortho");
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
}
void LineShader::draw(const std::vector<LineVertex> &vertex) {
    set4(u[0], context.camera->Ortho);
    attr(a[0], vertex, sizeof(LineVertex), offsetof(LineVertex, position));
    glDrawArrays(GL_LINE_STRIP, 0, vertex.size());
}
void LineShader::disable() {
    Shader::disable();
    glDisable(GL_BLEND);
}


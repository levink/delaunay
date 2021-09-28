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
void CircleShader::enable() const {
    Shader::enable();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void CircleShader::disable() const {
    Shader::disable();
    glDisable(GL_BLEND);
}
void CircleShader::draw(const std::vector<CircleModel>& items, const glm::vec3& color) {
    set4(u[0], context.camera->Ortho);
    set3(u[1], color);

    for(auto& item : items) {

        auto data = item.vertex;
        attr(a[0], data, sizeof(CircleVertex), offsetof(CircleVertex, position));
        attr(a[1], data, sizeof(CircleVertex), offsetof(CircleVertex, center));
        attr(a[2], data, sizeof(CircleVertex), offsetof(CircleVertex, radius));
        attr(a[3], data, sizeof(CircleVertex), offsetof(CircleVertex, fill));

        auto count = 6;
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, item.face);
    }
}
void CircleShader::draw(const DrawBatch& batch, const glm::vec3& color) {

    glBindBuffer(GL_ARRAY_BUFFER, batch.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.ibo);

    set4(u[0], context.camera->Ortho);
    set3(u[1], color);
    attr(a[0], sizeof(CircleVertex), offsetof(CircleVertex, position));
    attr(a[1], sizeof(CircleVertex), offsetof(CircleVertex, center));
    attr(a[2], sizeof(CircleVertex), offsetof(CircleVertex, radius));
    attr(a[3], sizeof(CircleVertex), offsetof(CircleVertex, fill));

    glDrawElements(GL_TRIANGLES, batch.count, GL_UNSIGNED_SHORT, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


LineShader::LineShader() : Shader(3, 4) {
    u[0] = Uniform("Ortho");
    u[1] = Uniform("Color");
    u[2] = Uniform("Width");
    a[0] = Attribute(VEC_2, "in_Position");
    a[1] = Attribute(VEC_2, "in_E1");
    a[2] = Attribute(VEC_2, "in_E2");
    a[3] = Attribute(VEC_2, "in_Offset");
}
void LineShader::link(const Camera *camera) {
    context.camera = camera;
}
void LineShader::enable() const {
    Shader::enable();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void LineShader::disable() const {
    Shader::disable();
    glDisable(GL_BLEND);
}
void LineShader::draw(const LineMesh& line, const glm::vec3& color, float width) {
    set4(u[0], context.camera->Ortho);
    set3(u[1], color);
    set1(u[2], width);

    auto data = line.vertex.data();
    attr(a[0], data, sizeof(LineVertex), offsetof(LineVertex, position));
    attr(a[1], data, sizeof(LineVertex), offsetof(LineVertex, e1));
    attr(a[2], data, sizeof(LineVertex), offsetof(LineVertex, e2));
    attr(a[3], data, sizeof(LineVertex), offsetof(LineVertex, offset));

    glDrawElements(GL_TRIANGLES, (int)(line.face.size() * 3u), GL_UNSIGNED_SHORT, line.face.data());
}


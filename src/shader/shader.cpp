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
void CircleShader::disable() const {
    Shader::disable();
    glDisable(GL_BLEND);
}

DrawBatch CircleShader::batch(const CircleItems& items) {

    GLuint vertexBuffer;
    auto vertexData = items.vertex.data();
    auto vertexSize = items.vertex.size() * sizeof(CircleVertex);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);

    GLuint ibo;
    auto faceData = items.face.data();
    auto faceSize = items.face.size() * sizeof(Face);
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceSize, faceData, GL_STATIC_DRAW);

    attr(a[0], sizeof(CircleVertex), offsetof(CircleVertex, position));
    attr(a[1], sizeof(CircleVertex), offsetof(CircleVertex, center));
    attr(a[2], sizeof(CircleVertex), offsetof(CircleVertex, radius));
    attr(a[3], sizeof(CircleVertex), offsetof(CircleVertex, fill));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    DrawBatch result;
    result.vbo = vertexBuffer;
    result.ibo = ibo;
    result.count = items.face.size() * 3;
    return result;
}




LineShader::LineShader() : Shader(2, 1) {
    u[0] = Uniform("Ortho");
    u[1] = Uniform("Color");
    a[0] = Attribute(VEC_2, "in_Position");
}
void LineShader::link(const Camera *camera) {
    context.camera = camera;
}
void LineShader::draw(const std::vector<LineVertex> &vertex, const glm::vec3& color) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.f);

    set4(u[0], context.camera->Ortho);
    set3(u[1], color);
    attr(a[0], vertex.data(), sizeof(LineVertex), offsetof(LineVertex, position));
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)vertex.size());

    glDisable(GL_BLEND);
    glLineWidth(1.f);
}


#include "shader.h"

CircleShader::CircleShader() : BaseShader(1, 4) {
    u[0] = Uniform("Ortho");
    a[0] = Attribute(VEC_2, "in_Position");
    a[1] = Attribute(VEC_3, "in_Color");
    a[2] = Attribute(VEC_2 ,"in_Center");
    a[3] = Attribute(FLOAT, "in_Radius");
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
        attr(a[0], item.vertices, sizeof(CircleVertex), offsetof(CircleVertex, position));
        attr(a[1], item.vertices, sizeof(CircleVertex), offsetof(CircleVertex, color));
        attr(a[2], item.vertices, sizeof(CircleVertex), offsetof(CircleVertex, center));
        attr(a[3], item.vertices, sizeof(CircleVertex), offsetof(CircleVertex, radius));

        glDrawElements(GL_TRIANGLES, (int)item.faces.size() * 3, GL_UNSIGNED_SHORT, item.faces.data());
    }
}
void CircleShader::disable() {
    BaseShader::disable();
    glDisable(GL_BLEND);
}


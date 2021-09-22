#include <src/model/circle.h>
#include "shader.h"

CircleShader::CircleShader() : BaseShader(1, 2) { }
void CircleShader::init() {
    a[0] = attr("in_Position");
    u[0] = uniform("ProjectionView");
    u[1] = uniform("Color");
}
void CircleShader::bind(const Camera* camera) {
    context.camera = camera;
}
void CircleShader::draw(const CircleModel& model) {
    
}


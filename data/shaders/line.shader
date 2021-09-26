#version 120
uniform mat4 Ortho;
uniform vec3 Color;

//#vertex
attribute vec2 in_Position;
void main() {
    gl_Position = Ortho * vec4(in_Position, 0.0, 1.0);
}

//#fragment
void main() {
    gl_FragColor = vec4(Color, 1.0);
}
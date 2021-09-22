#version 100
precision highp float;

uniform mat4 Ortho;
uniform vec4 Color;
attribute vec2 in_Position;

void main() {
	gl_Position = Ortho * vec4(in_Position, 0.0, 1.0);
}
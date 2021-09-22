#version 120
//precision highp float;

uniform mat4 Ortho;
uniform vec3 Color;
uniform vec2 Center;
uniform float Radius;

attribute vec2 in_Position;
varying vec2 Position;
void main() {
	gl_Position = Ortho * vec4(in_Position, 0.0, 1.0);
	Position = in_Position;
}
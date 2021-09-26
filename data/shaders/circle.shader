#version 120
uniform mat4 Ortho;
uniform vec3 Color;

//#vertex
attribute vec2 in_Position;
attribute vec2 in_Center;
attribute float in_Radius;
attribute float in_Fill;

varying vec2 Position;
varying vec2 Center;
varying float Radius;
varying float Fill;

void main() {
	gl_Position = Ortho * vec4(in_Position, 0.0, 1.0);
	Position = in_Position;
	Center = in_Center;
	Radius = in_Radius;
	Fill = in_Fill;
}


//#fragment
varying vec2 Position;
varying vec2 Center;
varying float Radius;
varying float Fill;

#define borderWidth 3.0
#define borderHalf 1.5
void main() {

	float inner = Radius - borderWidth;
	float middle = Radius - borderHalf;
	float dist = distance(Center, Position);
	float alpha = (Fill == 1.0 || dist > middle) ?
		smoothstep(Radius, middle, dist) :
		smoothstep(inner, middle, dist);
	gl_FragColor = vec4(Color.rgb, alpha);
}
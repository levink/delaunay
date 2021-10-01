#version 120
uniform mat4 Ortho;
uniform vec3 Color;

varying vec2 Position;
varying vec2 Center;
varying float Radius;
varying float Fill;

//#vertex
attribute vec2 in_Center;
attribute vec2 in_Offset;
attribute float in_Radius;
attribute float in_Fill;
void main() {

	vec2 position = in_Center + in_Offset * in_Radius;
	gl_Position = Ortho * vec4(position, 0.0, 1.0);

	Position = position;
	Center = in_Center;
	Radius = in_Radius;
	Fill = in_Fill;
}


//#fragment
void main() {
	const float width = 3.0;
	float inner = Radius - width;
	float middle = Radius - 0.5 * width;
	float dist = distance(Center, Position);
	float alpha = (Fill == 1.0 || dist > middle) ?
		smoothstep(Radius, middle, dist) :
		smoothstep(inner, middle, dist);
	gl_FragColor = vec4(Color.rgb, alpha);
}
#version 120
uniform mat4 Ortho;
uniform vec3 Color;
uniform vec2 Center;
uniform float Radius;

//#vertex
attribute vec2 in_Position;
varying vec2 Position;
void main() {
	gl_Position = Ortho * vec4(in_Position, 0.0, 1.0);
	Position = in_Position;
}

//#fragment
varying vec2 Position;
void main() {
	float thicknessHalf = 1.5;
	float inner = Radius - thicknessHalf * 2.0;
	float middle = Radius - thicknessHalf;

	float dist = distance(Center, Position);
	if (dist < inner) discard;
	if (dist > Radius) discard;

	float alpha = dist < middle ?
		smoothstep(inner, middle, dist):
		smoothstep(Radius, middle, dist);
	gl_FragColor = vec4(Color.rgb, alpha);
}
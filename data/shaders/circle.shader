#version 120
uniform mat4 Ortho;

//#vertex
attribute vec2 in_Position;
attribute vec3 in_Color;
attribute vec2 in_Center;
attribute float in_Radius;

varying vec2 Position;
varying vec3 Color;
varying vec2 Center;
varying float Radius;

void main() {
	gl_Position = Ortho * vec4(in_Position, 0.0, 1.0);
	Position = in_Position;
	Color = in_Color;
	Center = in_Center;
	Radius = in_Radius;
}


//#fragment
varying vec2 Position;
varying vec3 Color;
varying vec2 Center;
varying float Radius;
void main() {

	float thicknessHalf = 1.8;
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
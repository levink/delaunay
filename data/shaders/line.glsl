#version 120
uniform mat4 Ortho;
uniform vec3 Color;
uniform float Width;

varying vec2 Position;
varying vec2 E1;
varying vec2 E2;

//#vertex
attribute vec2 in_Position;
attribute vec2 in_E1;
attribute vec2 in_E2;
attribute vec2 in_Offset;
void main() {
    float w = Width * 0.5;
    vec2 dir = normalize(in_E2 - in_E1);
    vec2 up = vec2(-dir.y, dir.x);
    Position = in_Position +
        w * up * in_Offset.x +
        w * dir * in_Offset.y;

    gl_Position = Ortho * vec4(Position, 0.0, 1.0);
    E1 = in_E1;
    E2 = in_E2;
}

//#fragment
float getDistance(vec2 start, vec2 end, vec2 point) {
    vec2 dir = end - start;
    float length = dir.x * dir.x + dir.y * dir.y;
    float t = max(0.0, min(1.0, dot(point - start, dir) / length));
    vec2 proj = start + t * dir;
    return distance(point, proj);
}
void main() {
    float outer = Width * 0.5;
    float inner = outer - 1.8;
    float dist = getDistance(E1, E2, Position);
    float alpha = smoothstep(outer, inner, dist);
    gl_FragColor = vec4(Color.rgb, alpha);
}
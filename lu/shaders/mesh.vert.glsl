#version 450

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    gl_Position.x = positions[gl_VertexIndex].x;
    gl_Position.y = positions[gl_VertexIndex].y;
    gl_Position.z = 0.0;
    gl_Position.w = 1.0;
}


#version 450

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_tex;

layout(location = 0) out vec2 out_tex;

void main() {
    gl_Position = vec4(in_pos, 0.0, 1.0);
    out_tex = in_tex;
}

#version 450

layout(location = 0) in vec2 in_tex;
layout(binding = 0) uniform sampler2D tex_sampler;

layout(location = 0) out vec4 out_color;

void main() {

    float r = texture(tex_sampler, in_tex).r;
    out_color = vec4(1.0 * r, 0.0, 1.0 * r, 0.0);
}

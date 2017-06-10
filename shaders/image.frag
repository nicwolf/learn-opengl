#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

uniform sampler2D image;

out vec4 fragColor;

void main() {
    fragColor = texture(image, fs_in.uv);
}

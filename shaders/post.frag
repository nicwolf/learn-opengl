#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

uniform sampler2D colorbuffer;

out vec4 fragColor;

void main() {
    // Sample Color Buffer
    fragColor = texture(colorbuffer, fs_in.uv);
}

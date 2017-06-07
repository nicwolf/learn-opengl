#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

uniform sampler2D colorbuffer;
uniform float exposure;

out vec4 fragColor;

void main() {
    // Sample Color Buffer
    vec3 color = texture(colorbuffer, fs_in.uv).rgb;
    color = vec3(1.0) - exp(-color * exposure);
    fragColor = vec4(color, 1.0);
}

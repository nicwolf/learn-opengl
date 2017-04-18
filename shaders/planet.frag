#version 330 core
in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} fs_in;

uniform sampler2D texture_diffuse0;

out vec4 fragColor;

void main() {
    fragColor = texture(texture_diffuse0, fs_in.uv);
}

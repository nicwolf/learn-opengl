#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

uniform sampler2D image;

out float fragColor;

void main() {
    vec2 texelSize = 1.0 / textureSize(image, 0).xy;
    float color = 0.0;
    for (int x=-2; x<2; ++x) {
        for (int y=-2; y<2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            color += texture(image, fs_in.uv + offset).r;
        }
    }
    fragColor = color / 16.0;
}

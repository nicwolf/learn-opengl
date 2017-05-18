#version 330 core

in VS_OUT
{
    vec3 position;
    vec4 positionLightSpace;
    vec3 normal;
    vec2 uv;
} fs_in;

uniform sampler2D depthMap;
uniform sampler2D diffuse;

out vec4 fragColor;

void main() {
    vec3 test = fs_in.normal;
    test *= 0.5;
    test += 0.5;
    fragColor = vec4(texture(depthMap, fs_in.uv).rrr, 1.0);
}

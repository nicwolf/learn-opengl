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
    vec3 p = fs_in.positionLightSpace.xyz / fs_in.positionLightSpace.w;
    p *= 0.5;
    p += 0.5;
    fragColor = vec4(texture(depthMap, p.xy).rrr, 1.0);
}

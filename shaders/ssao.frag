#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

layout (std140) uniform Matrices
{
    uniform mat4 projectionMatrix;
    uniform mat4 viewMatrix;
}

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;

uniform vec3 kernelSamples[64];

out vec4 fragColor;

void main() {
    fragColor = texture(gAlbedoSpecular, fs_in.uv);
}

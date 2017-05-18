#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 modelMatrix;

layout (std140) uniform Matrices
{
    uniform mat4 projectionMatrix;
    uniform mat4 viewMatrix;
};

uniform mat4 lightSpaceMatrix;

out VS_OUT
{
    vec3 position;
    vec4 positionLightSpace;
    vec2 uv;
} vs_out;

void main() {
    mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
    mat4 modelView = viewMatrix * modelMatrix;
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    vs_out.position = vec3(modelView * vec4(position, 1.0));
    vs_out.positionLightSpace = lightSpaceMatrix * modelMatrix * vec4(position, 1.0);
    vs_out.uv = uv;
}

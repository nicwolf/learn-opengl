#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 instanceModelMatrix;

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewMatrixInverseTranspose;
uniform mat4 modelViewProjectionMatrix;

layout (std140) uniform Matrices
{
    uniform mat4 projectionMatrix;
    uniform mat4 viewMatrix;
};

out VS_OUT 
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * instanceModelMatrix * vec4(position, 1.0);
    vs_out.position = vec3(modelViewMatrix * vec4(position, 1.0));
    vs_out.normal = mat3(modelViewMatrixInverseTranspose) * normal;
    vs_out.uv = uv;
}

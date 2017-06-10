#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

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
    mat3 TBNMatrixInverse;
} vs_out;

void main() {
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    vs_out.position = vec3(modelViewMatrix * vec4(position, 1.0));
    vs_out.normal = mat3(modelViewMatrixInverseTranspose) * normal;
    vs_out.uv = uv;
    vec3 T = normalize(vec3(modelViewMatrix * vec4(tangent,   0.0)));
    vec3 B = normalize(vec3(modelViewMatrix * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(modelViewMatrix * vec4(normal,    0.0)));
    vs_out.TBNMatrixInverse = mat3(T, B, N);
}

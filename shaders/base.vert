#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

layout (std140) uniform Matrices
{
    uniform mat4 projectionMatrix;
    uniform mat4 viewMatrix;
};

out VS_OUT 
{
    vec3 position;
} vs_out;

void main()
{
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    vs_out.position = vec3(modelViewMatrix * vec4(position, 1.0));
}

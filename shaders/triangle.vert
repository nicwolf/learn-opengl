#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform vec3 offset;

out vec3 vertexColor;
out vec3 vertexPosition;

void main() {
    gl_Position = vec4(position + offset, 1.0);
    vertexColor = color;
    vertexPosition = gl_Position.xyz;
}

#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

uniform vec3 offset;
uniform mat4 transform;

out vec3 vertexColor;
out vec3 vertexPosition;
out vec2 textureCoordinate;

void main() {
    gl_Position = transform * vec4(position + offset, 1.0);
    vertexColor = color;
    vertexPosition = gl_Position.xyz;
    textureCoordinate = vec2(texCoord.s, 1.0f - texCoord.t);
}

#version 330 core
in vec3 vertexColor;
in vec3 vertexPosition;
in vec2 textureCoordinate;

uniform sampler2D containerTexture;
uniform sampler2D faceTexture;

out vec4 color;

void main() {
   color = mix(texture(containerTexture, textureCoordinate),
               texture(faceTexture     , textureCoordinate), 0.2);
}
#version 330 core
in vec3 vertexColor;
in vec3 vertexPosition;
in vec2 textureCoordinate;

uniform vec3      objectColor;
uniform vec3      lightColor;
uniform sampler2D containerTexture;
uniform sampler2D faceTexture;
uniform float     mixConstant;

out vec4 color;

void main() {
    color = vec4(lightColor * objectColor, 1.0f);
//   color = mix(texture(containerTexture, textureCoordinate),
//               texture(faceTexture     , textureCoordinate), mixConstant);
}

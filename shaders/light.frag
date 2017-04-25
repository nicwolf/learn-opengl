#version 330 core
out vec4 fragColor;

layout (std140) uniform Lighting {
    uniform vec3 lightPos;
    uniform vec3 lightColor;
};

void main() {
   fragColor = vec4(lightColor, 1.0);
}

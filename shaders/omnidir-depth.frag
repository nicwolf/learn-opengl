#version 330 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float near;
uniform float far;

void main() {
    float d = length(fragPos.xyz - lightPos);
    d = (d - near) / (far - near);
    gl_FragDepth = d;
}

#version 330 core

in VS_OUT
{
    vec3 position;
} fs_in;

uniform vec3 color;

out vec4 fragColor;

void main()
{
    fragColor = vec4(color, 1.0);
}

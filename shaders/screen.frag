#version 330 core

uniform sampler2D screenColorBuffer;

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

out vec4 fragColor;

void main()
{
    fragColor = texture(screenColorBuffer, fs_in.uv);
}

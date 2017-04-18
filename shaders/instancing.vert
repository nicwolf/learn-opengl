#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 instanceMatrix;

layout (std140) uniform Matrices
{
    uniform mat4 view;
    uniform mat4 projection;
};

out VS_OUT
{
    vec2 uv;

} vs_out;

uniform mat4 model;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(position, 1.0);
    vs_out.uv = uv;
}

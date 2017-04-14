#version 330 core
layout (location=0) in vec3 position;

layout (std140) uniform Matrices {
    uniform mat4 view;
    uniform mat4 projection;
};


out vec3 uv;

void main()
{
    mat4 v = mat4(mat3(view));
    gl_Position = (projection * v * vec4(position, 1.0)).xyww;
    uv = position;
}

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;

layout (std140) uniform Matrices {
    uniform mat4 view;
    uniform mat4 projection;
};

out vec3 Normal;

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    Normal = normalMatrix * normal;
}

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrixInverseTranspose;
uniform mat4 modelViewProjection;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main()
{
    gl_Position     = projection * view * model * vec4(position, 1.0);
    vs_out.position = vec3(view * model * vec4(position, 1.0));
    vs_out.normal   = mat3(inverse(transpose(view * model))) * normal;
    vs_out.uv       = uv;
}

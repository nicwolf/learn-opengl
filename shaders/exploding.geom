#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float time;

vec4 explode(vec4 p, vec3 n);

void main()
{
    vec4 a = gl_in[0].gl_Position;
    vec4 b = gl_in[1].gl_Position;
    vec4 c = gl_in[2].gl_Position;

    vec3 normal = normalize(cross(vec3(a-b), vec3(c-b)));

    gl_Position = explode(a, normal);
    EmitVertex();

    gl_Position = explode(b, normal);
    EmitVertex();

    gl_Position = explode(c, normal);
    EmitVertex();

}

vec4 explode(vec4 p, vec3 n)
{
    float mag = (sin(time) + 1.0f) * 0.50f;
    return p + vec4(mag * n, 0.0f);
}

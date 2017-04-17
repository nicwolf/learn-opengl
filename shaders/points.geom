#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT
{
    vec3 color;
} gs_in[];

out vec3 fColor;

void main()
{
    fColor = gs_in[0].color;

    vec4 position = gl_in[0].gl_Position;

    gl_Position = position + vec4(-0.2f, -0.2f, 0.0f, 0.0f);
    EmitVertex();

    gl_Position = position + vec4( 0.2f, -0.2f, 0.0f, 0.0f);
    EmitVertex();

    gl_Position = position + vec4(-0.2f,  0.2f, 0.0f, 0.0f);
    EmitVertex();

    gl_Position = position + vec4( 0.2f,  0.2f, 0.0f, 0.0f);
    EmitVertex();

    fColor = vec3(1.0);
    gl_Position = position + vec4( 0.0f,  0.4f, 0.0f, 0.0f);
    EmitVertex();

    EndPrimitive();
}

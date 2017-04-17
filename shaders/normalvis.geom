#version 330

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT
{
    vec3 normal;
} gs_in[];

void main()
{
    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;
    vec4 p3 = gl_in[2].gl_Position;

    vec3 e1 = vec3(p1-p2);
    vec3 e2 = vec3(p3-p2);
    vec3 normal = gs_in[0].normal;
    float mag = 0.5f;

    gl_Position = p1;
    EmitVertex();

    gl_Position = p1 + vec4((mag * normal), 0.0);
    EmitVertex();

    EndPrimitive();

    gl_Position = p2;
    EmitVertex();

    gl_Position = p2 + vec4((mag * normal), 0.0);
    EmitVertex();

    EndPrimitive();

    gl_Position = p3;
    EmitVertex();

    gl_Position = p3 + vec4((mag * normal), 0.0);
    EmitVertex();

    EndPrimitive();
}

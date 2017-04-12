#version 330 core
in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform samplerCube skybox;
uniform vec3 cameraPos;

uniform float near;
uniform float far;

float linearizeDepth(float d);

void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    color = texture(skybox, R);
}

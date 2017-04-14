#version 330 core
in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform samplerCube skybox;
uniform vec3 cameraPos;
uniform float cameraYaw;
uniform mat4 view;
uniform float near;
uniform float far;

float linearizeDepth(float d);

void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
//    R = vec3(inverse(view) * vec4(R, 0.0)); // If we were calculating in eyespace, we would have to convert view coords back to world coords.
    color = texture(skybox, R);
}

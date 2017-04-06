#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;

uniform float near;
uniform float far;

float linearizeDepth(float d);

void main()
{
    color = texture(texture1, TexCoords);
//    float depth = linearizeDepth(gl_FragCoord.z);
//    color = vec4(vec3(depth), 1.0);
}

float linearizeDepth(float d) {
    float z = 1 / ((d / far) - (d / near) + (1 / near));
    float d_linear = (z - near) / (far - near);
    return d_linear;
}

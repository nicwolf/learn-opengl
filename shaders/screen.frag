#version 330 core

uniform sampler2D screenColorBuffer;

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

float linearizeDepth(float depth);
const float near = 0.10f;
const float far  = 10.0f;

out vec4 fragColor;

void main()
{
    float depth = texture(screenColorBuffer, fs_in.uv).r;
    fragColor = vec4(vec3(linearizeDepth(depth)), 1.0);
//    fragColor = texture(screenColorBuffer, fs_in.uv);
}

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - z * (far - near));
    return linearDepth;
}

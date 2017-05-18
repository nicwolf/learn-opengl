#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

uniform sampler2D colorbuffer;

const float far = 15.0f;
const float near = 0.1f;
float linearizeDepth(float depth);

out vec4 fragColor;

void main() {
//    float depth = texture(colorbuffer, fs_in.uv).r;
//    fragColor = vec4(vec3(linearizeDepth(depth) / far), 1.0);
    fragColor = texture(colorbuffer, fs_in.uv);
//    fragColor = vec4(1.0);
}

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - z * (far - near));
    return linearDepth;
}

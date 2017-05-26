#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

uniform samplerCube depthMap;
uniform float far;
uniform float near;

float linearizeDepth(float depth);

out vec4 fragColor;

void main() {
//    fragColor = vec4(vec3(texture(depthMap, fs_in.uv).r), 1.0);
    float depth = texture(depthMap, vec3(fs_in.uv, 0.0)).r;
    fragColor = vec4(vec3(linearizeDepth(depth)), 1.0);
//    fragColor = texture(depthMap, fs_in.uv);
}

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - z * (far - near));
    return linearDepth;
}

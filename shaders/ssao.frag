#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

layout (std140) uniform Matrices
{
    uniform mat4 projectionMatrix;
    uniform mat4 viewMatrix;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;

const int KERNEL_SIZE = 32;
uniform vec3 kernelSamples[KERNEL_SIZE];
uniform sampler2D kernelRotationTexture;

out float fragColor;

const vec2 noiseScale = vec2(800.0 / 4.0, 600.0 / 4.0);

void main() {
    vec3 fragPos = texture(gPosition, fs_in.uv).xyz;
    vec3 fragNormal = texture(gNormal, fs_in.uv).xyz;
    vec3 rVec = texture(kernelRotationTexture, fs_in.uv * noiseScale).xyz;
    vec3 tangent = normalize((rVec - fragNormal * dot(rVec, fragNormal)));
    vec3 bitangent = cross(fragNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, fragNormal);
    float occlusion = 0.0;
    for (int i=0; i<KERNEL_SIZE; ++i) {
        vec3 kernelSample = TBN * kernelSamples[i];
        float radius = 0.5;
        kernelSample *= radius; // Radius
        kernelSample += fragPos;
        vec4 offset = vec4(kernelSample, 1.0);
        offset = projectionMatrix * offset;
        offset.xyz /= offset.w;
        offset.xyz *= 0.5;
        offset.xyz += 0.5;
        float sampleDepth = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= kernelSample.z + 0.025 ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / KERNEL_SIZE);
    fragColor = occlusion;
}

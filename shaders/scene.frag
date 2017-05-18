#version 330 core

in VS_OUT
{
    vec3 position;
    vec4 positionLightSpace;
    vec2 uv;
} fs_in;

const float far = 15.0f;
const float near = 0.1f;
float linearizeDepth(float depth);

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
};

uniform Material material;

uniform sampler2D depthMap;

out vec4 fragColor;

void main() {
    vec3 p = fs_in.positionLightSpace.xyz / fs_in.positionLightSpace.w;
    p *= 0.5;
    p += 0.5;
    float closestDepth = texture(depthMap, p.xy).r;
    float fragmentDepth = p.z;
    float shadow = 0.0;
    if (fragmentDepth <= closestDepth) {
        shadow = 1.0;
    }
    fragColor = texture(material.diffuse, fs_in.uv);
    fragColor -= (1.0 - shadow);
    fragColor = vec4(texture(depthMap, p.xy).rrr, 1.0);
}

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - z * (far - near));
    return linearDepth;
}

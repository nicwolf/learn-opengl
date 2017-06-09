#version 330 core

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

struct Light
{
    vec3 position;
    vec3 color;
    float constFalloff;
    float linFalloff;
    float quadFalloff;
};

layout (std140) uniform Matrices
{
    uniform mat4 projectionMatrix;
    uniform mat4 viewMatrix;
};

const int NR_LIGHTS = 30;
uniform Light lights[NR_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;

vec3 calcLight(Light light, vec3 fragPosition, vec3 fragNormal, vec3 fragAlbedo);

out vec4 fragColor;

void main() {
    vec3 fragPosition  = texture(gPosition, fs_in.uv).rgb;
    vec3 fragNormal    = texture(gNormal, fs_in.uv).rgb;
    vec3 fragAlbedo    = texture(gAlbedoSpecular, fs_in.uv).rgb;
    float fragSpecular = texture(gAlbedoSpecular, fs_in.uv).a;

    vec3 color = vec3(0.0);
    for (int i=0; i<NR_LIGHTS; ++i) {
        color += calcLight(lights[i], fragPosition, fragNormal, fragAlbedo);
    }
    fragColor = vec4(color, 1.0);
}

vec3 calcLight(Light light, vec3 fragPosition, vec3 fragNormal, vec3 fragAlbedo) {
    vec3 lightPosition = (viewMatrix * vec4(light.position, 1.0)).xyz;
    vec3 lightDir = normalize(lightPosition - fragPosition);

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * fragAlbedo;

    // Diffuse
    float diffuseStrength;
    diffuseStrength = dot(lightDir, fragNormal);
    diffuseStrength = max(diffuseStrength, 0.0);
    vec3 diffuse = light.color * diffuseStrength * fragAlbedo;

    // Attenuation
    float distance    = length(lightPosition - fragPosition);
    float attenuation = 1.0 / (light.constFalloff
                               + light.linFalloff * distance
                               + light.quadFalloff * pow(distance, 2.0));
    ambient  *= attenuation;
    diffuse  *= attenuation;

    vec3 result = ambient + diffuse;
    return result;
}

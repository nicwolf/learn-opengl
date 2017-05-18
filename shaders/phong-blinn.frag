#version 330 core

float near = 0.1f;
float far  = 25.0f;

in VS_OUT
{
    vec3 position;
    vec4 positionLightSpace;
    vec3 normal;
    vec2 uv;
} fs_in;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
};

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir);

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);

struct ConeLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutoff;
    float outerCutoff;

    float constantFalloff;
    float linearFalloff;
    float quadraticFalloff;
};
vec3 calcConeLight(ConeLight light, vec3 normal, vec3 viewDir);

layout (std140) uniform Lights
{
    uniform PointLight pointLights[4];
    uniform DirectionalLight dirLight;
    uniform ConeLight coneLight;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;

uniform sampler2D depthMap;
float calcShadow();

out vec4 fragColor;

void main() {
    vec3 viewDir = normalize(-fs_in.position);
    vec3 normal  = normalize(fs_in.normal);
    vec3 result = vec3(0.0);
    result += calcDirLight(dirLight, normal, viewDir);
    for (int i = 0; i < 4; i++) {
        result += calcPointLight(pointLights[i], normal, viewDir);
    }
    result += calcConeLight(coneLight, normal, viewDir);
    fragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.uv));

    // Diffuse
    float diffuseStrength;
    vec3 diffuseMap = vec3(texture(material.diffuse, fs_in.uv));
    diffuseStrength = dot(lightDir, normal);
    diffuseStrength = max(diffuseStrength, 0.0);
    vec3 diffuse    = diffuseStrength * light.diffuse * diffuseMap;

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float specularStrength;
    specularStrength = dot(normal, halfwayDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
    vec3 specularMap = vec3(texture(material.specular, fs_in.uv));
    vec3 specular    = specularStrength * light.specular * specularMap;

    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 calcConeLight(ConeLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    float theta = dot(lightDir, viewDir);
    float epsilon = light.cutoff - light.outerCutoff;
    float rolloff = (theta - light.outerCutoff) / epsilon;
    rolloff = clamp(rolloff, 0.0, 1.0);

    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.uv));

    // Diffuse
    float diffuseStrength;
    vec3 diffuseMap = vec3(texture(material.diffuse, fs_in.uv));
    diffuseStrength = dot(lightDir, normal);
    diffuseStrength = max(diffuseStrength, 0.0);
    vec3 diffuse = light.diffuse * diffuseStrength * diffuseMap;

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float specularStrength;
    specularStrength = dot(normal, halfwayDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
    vec3 specularMap = vec3(texture(material.specular, fs_in.uv));
    vec3 specular = light.specular * specularStrength * specularMap;

    // Attenuation
    float distance    = length(light.position - fs_in.position);
    float attenuation = 1.0 / (light.constantFalloff
                               + light.linearFalloff * distance
                               + light.quadraticFalloff * pow(distance, 2.0));
    ambient  *= attenuation * rolloff;
    diffuse  *= attenuation * rolloff;
    specular *= attenuation * rolloff;

    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fs_in.position);

    // Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.uv));

    // Diffuse
    float diffuseStrength;
    vec3 diffuseMap = vec3(texture(material.diffuse, fs_in.uv));
    diffuseStrength = dot(lightDir, normal);
    diffuseStrength = max(diffuseStrength, 0.0);
    vec3 diffuse = light.diffuse * diffuseStrength * diffuseMap;

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    float specularStrength;
    specularStrength = dot(normal, halfwayDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
    vec3 specularMap = vec3(texture(material.specular, fs_in.uv));
    vec3 specular = light.specular * specularStrength * specularMap;

    // Attenuation
    float distance    = length(light.position - fs_in.position);
    float attenuation = 1.0 / (light.constantFalloff
                               + light.linearFalloff * distance
                               + light.quadraticFalloff * pow(distance, 2.0));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    return result;
}

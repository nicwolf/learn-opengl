#version 330 core

in VS_OUT
{
    vec3 position;
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
    float quadFalloff;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir);

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight dirLight;
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);

struct ConeLight {
    vec3 position;
    vec3 direction;

    float cutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constantFalloff;
    float linearFalloff;
    float quadFalloff;
};
uniform ConeLight coneLight;
vec3 calcConeLight(ConeLight light, vec3 normal, vec3 viewDir);

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;

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
    vec3 lightReflectDir  = reflect(-lightDir, normal);
    float specularStrength;
    vec3 specularMap = vec3(texture(material.specular, fs_in.uv));
    specularStrength = dot(viewDir, lightReflectDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
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
    vec3 lightReflectDir = reflect(-lightDir, normal);
    float specularStrength;
    specularStrength = dot(viewDir, lightReflectDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
    vec3 specularMap = vec3(texture(material.specular, fs_in.uv));
    vec3 specular = light.specular * specularStrength * specularMap;

    // Attenuation
    float distance    = length(light.position - fs_in.position);
    float attenuation = 1.0 / (light.constantFalloff
                               + light.linearFalloff * distance
                               + light.quadFalloff * pow(distance, 2.0));
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
    vec3 lightReflectDir = reflect(-lightDir, normal);
    float specularStrength;
    specularStrength = dot(viewDir, lightReflectDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
    vec3 specularMap = vec3(texture(material.specular, fs_in.uv));
    vec3 specular = light.specular * specularStrength * specularMap;

    // Attenuation
    float distance    = length(light.position - fs_in.position);
    float attenuation = 1.0 / (light.constantFalloff
                               + light.linearFalloff * distance
                               + light.quadFalloff * pow(distance, 2.0));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    return result;
}

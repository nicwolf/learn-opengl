#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constantFalloff;
    float linearFalloff;
    float quadFalloff;
};
uniform PointLight pointLight;

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight dirLight;

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

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;
uniform mat4 view;

out vec4 color;

void main() {
    // Lighting
    // --------
    // Administrivia
    vec3 lightDir = -coneLight.direction;
    vec3 viewDir  = normalize(-FragPos);
    vec3 normal   = normalize(Normal);

    float theta = dot(lightDir, viewDir);
    float epsilon = coneLight.cutoff - coneLight.outerCutoff;
    float intensity = (theta - coneLight.outerCutoff) / epsilon;
    intensity = clamp(intensity, 0.0, 1.0);
    vec3 result = vec3(0.0);
    // Diffuse
    float diffuseStrength;
    vec3 diffuseMap = vec3(texture(material.diffuse, TexCoord));
    diffuseStrength = dot(lightDir, normal);
    diffuseStrength = max(diffuseStrength, 0.0);
    vec3 diffuse = coneLight.diffuse * diffuseStrength * diffuseMap;

    // Specular
    vec3 lightReflectDir = reflect(-lightDir, normal);
    float specularStrength;
    specularStrength = dot(viewDir, lightReflectDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
    vec3 specularMap = vec3(texture(material.specular, TexCoord));
    vec3 specular = coneLight.specular * specularStrength * specularMap;

    // Emission
    // vec3 emission = vec3(texture(material.emission, TexCoord));

    // Attenuation
    float distance    = length(coneLight.position - FragPos);
    float attenuation = 1.0 / (coneLight.constantFalloff
                               + coneLight.linearFalloff * distance
                               + coneLight.quadFalloff * pow(distance, 2.0));
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    // vec3 result = diffuse;
    result = diffuse + specular; // + emission;
    color = vec4(result, 1.0f);
}

#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

out vec4 color;

void main() {
    // Lighting
    // --------
    // Administrivia
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 viewDir  = normalize(-FragPos);
    vec3 normal   = normalize(Normal);

    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    float diffuseStrength;
    diffuseStrength = dot(lightDir, normal);
    diffuseStrength = max(diffuseStrength, 0.0);
    vec3 diffuse = light.diffuse * diffuseStrength * material.diffuse;

    // Specular
    vec3 lightReflectDir = reflect(-lightDir, normal);
    float specularStrength;
    specularStrength = dot(viewDir, lightReflectDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
    vec3 specular = light.specular * specularStrength * material.specular;

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0f);
}

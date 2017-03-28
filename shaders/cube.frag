#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec2 TexCoord;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;
uniform sampler2D container;

out vec4 color;

void main() {
    // Lighting
    // --------
    // Administrivia
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 viewDir  = normalize(-FragPos);
    vec3 normal   = normalize(Normal);

    // Diffuse
    float diffuseStrength;
    diffuseStrength = dot(lightDir, normal);
    diffuseStrength = max(diffuseStrength, 0.0);
    vec3 diffuseMap = vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diffuseStrength * diffuseMap;

    // Specular
    vec3 lightReflectDir = reflect(-lightDir, normal);
    float specularStrength;
    specularStrength = dot(viewDir, lightReflectDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, material.shininess);
    vec3 specularMap = vec3(texture(material.specular, TexCoord));
    vec3 specular = light.specular * specularStrength * specularMap;

//    vec3 result = diffuse;
    vec3 result = diffuse + specular;
    color = vec4(result, 1.0f);
}

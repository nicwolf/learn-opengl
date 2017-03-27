#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

uniform vec3      objectColor;
uniform vec3      lightColor;
//uniform sampler2D containerTexture;
//uniform sampler2D faceTexture;
//uniform float     mixConstant;

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
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float diffuseStrength;
    diffuseStrength = dot(lightDir, normal);
    diffuseStrength = max(diffuseStrength, 0.0);
    vec3 diffuse = diffuseStrength * lightColor;

    // Specular
    vec3 lightReflectDir = reflect(-lightDir, normal);
    float specularStrength;
    specularStrength = dot(viewDir, lightReflectDir);
    specularStrength = max(specularStrength, 0.0);
    specularStrength = pow(specularStrength, 32);
    vec3 specular = specularStrength * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0f);
//   color = mix(texture(containerTexture, textureCoordinate),
//               texture(faceTexture     , textureCoordinate), mixConstant);
}

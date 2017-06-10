#version 330 core

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 albedoSpecular;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    mat3 TBNMatrixInverse;
} fs_in;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D depth;
    float shininess;
};
uniform Material material;

void main() {
    position = fs_in.position;
    // position = vec3(0.5);
//    normal = normalize(fs_in.normal);
    normal = texture(material.normal, fs_in.uv).xyz;
    normal = fs_in.TBNMatrixInverse * normal;
    normal = normalize(normal);
    // To Do: Parallax Mapping
    albedoSpecular.rgb = texture(material.diffuse, fs_in.uv).rgb;
    albedoSpecular.a   = texture(material.specular, fs_in.uv).r;
}

#version 330 core

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 albedoSpecular;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    mat3 TBNMatrix;
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

vec2 parallaxMapping();

void main() {
    position = fs_in.position;
    vec2 uv = parallaxMapping();
//    if (uv.x > 1.0 || uv.x < 0.0 || uv.y > 1.0 || uv.y < 0.0) discard;
    normal = texture(material.normal, uv).xyz;
    normal = fs_in.TBNMatrixInverse * normal;
    normal = normalize(normal);
    // To Do: Parallax Mapping
    albedoSpecular.rgb = texture(material.diffuse, uv).rgb;
    albedoSpecular.a   = texture(material.specular, uv).r;
}

vec2 parallaxMapping() {
    float numLayers = 30.0;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    float currentMapDepth = texture(material.depth, fs_in.uv).r;

    vec3 viewDir = fs_in.TBNMatrix * fs_in.position;
    float heightScale = .1;
    vec2 p = viewDir.xy * heightScale;
    vec2 deltaUV = p / numLayers;
    vec2 currentUV = fs_in.uv;

    while (currentLayerDepth < currentMapDepth) {
        currentLayerDepth += layerDepth;
        currentUV -= deltaUV;
        currentMapDepth = texture(material.depth, currentUV).r;
    }

    float previousLayerDepth = currentLayerDepth - layerDepth;
    vec2 previousUV = currentUV + deltaUV;
    float previousMapDepth = texture(material.depth, previousUV).r;

    float afterDepth = currentMapDepth - currentLayerDepth;
    float beforeDepth = previousMapDepth - previousLayerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);

    return previousUV*weight + currentUV*(1-weight);
}

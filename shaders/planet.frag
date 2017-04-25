#version 330 core
in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} fs_in;

layout (std140) uniform Lighting {
    uniform vec3 lightPos;
    uniform vec3 lightColor;
};

layout (std140) uniform Matrices {
    uniform mat4 view;
    uniform mat4 projection;
};

uniform sampler2D texture_diffuse0;

out vec4 fragColor;

void main() {
    vec3 eyeLightPos = vec3(view * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(eyeLightPos - fs_in.position);
    float diffuseStrength = dot(fs_in.normal, lightDir);
    vec4 diffuseTexture = texture(texture_diffuse0, fs_in.uv);
    fragColor = diffuseTexture * diffuseStrength;
}

#version 330 core

out vec4 fragColor;

in VS_OUT
{
    vec3 position;
    vec2 uv;
} fs_in;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 uvOffset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, fs_in.uv).rgb * weight[0];
    if (horizontal) {
        for (int i=1; i<5; ++i) {
            result += texture(image, fs_in.uv + vec2(uvOffset.s * i, 0.0)).rgb * weight[i];
            result += texture(image, fs_in.uv - vec2(uvOffset.s * i, 0.0)).rgb * weight[i];
        }
    } else {
        for (int i=1; i<5; ++i) {
            result += texture(image, fs_in.uv + vec2(0.0, uvOffset.t * i)).rgb * weight[i];
            result += texture(image, fs_in.uv - vec2(0.0, uvOffset.t * i)).rgb * weight[i];
        }
    }
//    fragColor = texture(image, fs_in.uv);
    fragColor = vec4(result, 1.0);
//    fragColor = vec4(1.0);
}

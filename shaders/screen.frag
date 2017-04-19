#version 330 core

in vec2 uv;

const float offset = 1.0 / 300.0;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main()
{
    vec4 texColor  = texture(screenTexture, uv);

    // Invert
    // fragColor = vec4(vec3(1.0 - texColor), 1.0);

    // Naive Grayscale
    // float avg = (texColor.r + texColor.g + texColor.b) / 3;
    // fragColor = vec4(vec3(avg), 1.0);

    // Accurate Grayscale
    // avg = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
    // fragColor = vec4(vec3(avg), 1.0);

    // Kernel Effects
    // --------------
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2(   0.0f,  offset),
        vec2( offset,  offset),
        vec2(-offset,    0.0f),
        vec2(   0.0f,    0.0f),
        vec2( offset,    0.0f),
        vec2(-offset, -offset),
        vec2(   0.0f, -offset),
        vec2( offset, -offset)
    );
    vec3 sampleTex[9];
    vec3 convolvedColor = vec3(0.0);

    // Sharpen
    float sharpenKernel[9] = float[] (
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, uv.st + offsets[i]));
    }
    for (int i = 0; i < 9; i++)
    {
        convolvedColor += sampleTex[i] * sharpenKernel[i];
    }
    fragColor = vec4(convolvedColor, 1.0);

    // Blur
//    float blurKernel[9] = float[] (
//        1.0 / 16, 2.0 / 16, 1.0 / 16,
//        2.0 / 16, 4.0 / 16, 2.0 / 16,
//        1.0 / 16, 2.0 / 16, 1.0 / 16
//    );
//    for (int i = 0; i < 9; i++)
//    {
//        sampleTex[i] = vec3(texture(screenTexture, uv.st + offsets[i]));
//    }
//    for (int i = 0; i < 9; i++)
//    {
//        convolvedColor += sampleTex[i] * blurKernel[i];
//    }

//    fragColor = vec4(convolvedColor, 1.0);

    // Edge Detection
//    float edgeKernel[9] = float[] (
//        1.0,  1.0, 1.0,
//        1.0, -8.0, 1.0,
//        1.0,  1.0, 1.0
//    );
//    for (int i = 0; i < 9; i++)
//    {
//        sampleTex[i] = vec3(texture(screenTexture, uv.st + offsets[i]));
//    }
//    for (int i = 0; i < 9; i++)
//    {
//        convolvedColor += sampleTex[i] * edgeKernel[i];
//    }
//    fragColor = vec4(convolvedColor, 1.0);

    // Solid Color
//     fragColor = vec4(1.0);

//    fragColor = texColor;
}

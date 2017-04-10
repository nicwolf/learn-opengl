#version 330 core
in vec2 TexCoords;

uniform sampler2D texture1;

out vec4 fragColor;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    fragColor = texColor;
//    fragColor = vec4(1.0);
}

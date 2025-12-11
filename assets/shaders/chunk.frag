#version 450 core
out vec4 outputColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    outputColor = texture(ourTexture, TexCoord);
}
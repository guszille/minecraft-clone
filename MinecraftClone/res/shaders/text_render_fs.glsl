#version 460 core

in vec2 ioTexCoords;

uniform sampler2D uText;
uniform vec3 uTextColor;

out vec4 Color;

void main()
{
    Color = vec4(uTextColor, 1.0) * vec4(1.0, 1.0, 1.0, texture(uText, ioTexCoords).r);
}

#version 460 core

in vec2 ioTexCoords;
in vec3 ioColor;

uniform sampler2D uTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(uTexture, ioTexCoords) * vec4(ioColor, 1.0);
}

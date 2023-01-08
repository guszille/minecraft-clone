#version 460 core

in vec2 ioTexCoords;

uniform sampler2D uDepthMap;

out vec4 FragColor;

void main()
{
    float depthValue = texture(uDepthMap, ioTexCoords).r;

    FragColor = vec4(vec3(depthValue), 1.0);
}

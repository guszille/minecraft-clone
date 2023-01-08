#version 460 core

in float ioAmbientOcclusion;

uniform float uOcclusionStrength = 1.0;

out vec4 FragColor;

void main()
{
    float occlusionFactor = ioAmbientOcclusion * uOcclusionStrength;

    FragColor = vec4((1.0 - occlusionFactor) * vec3(1.0), 1.0);
}

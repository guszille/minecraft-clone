#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in float aAmbientOcclusion;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out float ioAmbientOcclusion;

void main()
{
    ioAmbientOcclusion = aAmbientOcclusion;

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
}

#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uLightSpaceMatrix; // A.K.A. view and projections matrices combined.

out vec2 ioTexCoords;

void main()
{
    ioTexCoords = aTexCoords;

    gl_Position = uLightSpaceMatrix * uModelMatrix * vec4(aPos, 1.0);
}

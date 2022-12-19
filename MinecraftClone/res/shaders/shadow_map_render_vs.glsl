#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModelMatrix;
uniform mat4 uLightSpaceMatrix; // A.K.A. view and projections matrices combined.

void main()
{
    gl_Position = uLightSpaceMatrix * uModelMatrix * vec4(aPos, 1.0);
}

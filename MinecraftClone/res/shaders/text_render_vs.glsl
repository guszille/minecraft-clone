#version 460 core

layout (location = 0) in vec4 aVertex; // <vec2 aPos, vec2 aTexCoords>

uniform mat4 uProjectionMatrix;

out vec2 ioTexCoords;

void main()
{
    gl_Position = uProjectionMatrix * vec4(aVertex.xy, 0.0, 1.0);

    ioTexCoords = aVertex.zw;
}

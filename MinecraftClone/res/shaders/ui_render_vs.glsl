#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;

out vec2 ioTexCoords;

void main()
{
	gl_Position = uProjectionMatrix * uModelMatrix * vec4(aPos.xy, 0.0, 1.0);

	ioTexCoords = aTexCoords;
}
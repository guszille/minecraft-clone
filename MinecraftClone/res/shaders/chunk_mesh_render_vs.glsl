#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aDebugColor;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uLightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosViewSpace;
    vec4 FragPosLightSpace;
    vec3 DebugColor;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(uModelMatrix))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosViewSpace = uViewMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.FragPosLightSpace = uLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.DebugColor = aDebugColor;

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.FragPos, 1.0);
}

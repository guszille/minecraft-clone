#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aOcclusion;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uLightSpaceMatrix;

out VS_OUT {
    vec3 FragPosModelSpace;
    vec3 FragPosViewSpace;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    float Occlusion;
} vs_out;

void main()
{
    vs_out.FragPosModelSpace = vec3(uModelMatrix * vec4(aPos, 1.0));
    vs_out.FragPosViewSpace = vec3(uViewMatrix * vec4(vs_out.FragPosModelSpace, 1.0));
    vs_out.Normal = transpose(inverse(mat3(uModelMatrix))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = uLightSpaceMatrix * vec4(vs_out.FragPosModelSpace, 1.0);
    vs_out.Occlusion = aOcclusion;

    gl_Position = uProjectionMatrix * vec4(vs_out.FragPosViewSpace, 1.0);
}

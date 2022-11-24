#pragma once

#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader(const char* vertexShaderFilepath, const char* fragmentShaderFilepath);
	Shader(const char* vertexShaderFilepath, const char* geometryShaderFilepath, const char* fragmentShaderFilepath);
	~Shader();

	void Bind();
	void Unbind();

	void SetUniform1i(const char* uniformName, int data);

	void SetUniform3f(const char* uniformName, const glm::vec3& data);

	void SetUniformMatrix4fv(const char* uniformName, const glm::mat4& data);

private:
	unsigned int m_ShaderProgramID;

	const unsigned int CreateShader(const char* filepath, int shaderType);
};

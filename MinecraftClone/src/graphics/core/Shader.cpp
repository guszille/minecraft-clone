#include "Shader.h"

Shader::Shader(const char* vertexShaderFilepath, const char* fragmentShaderFilepath)
	: m_ShaderProgramID()
{
	int success;
	char infoLog[512];

	unsigned int vertexShaderID = CreateShader(vertexShaderFilepath, GL_VERTEX_SHADER);
	unsigned int fragmentShaderID = CreateShader(fragmentShaderFilepath, GL_FRAGMENT_SHADER);

	m_ShaderProgramID = glCreateProgram();

	glAttachShader(m_ShaderProgramID, vertexShaderID);
	glAttachShader(m_ShaderProgramID, fragmentShaderID);

	glLinkProgram(m_ShaderProgramID);
	glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(m_ShaderProgramID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Linkage failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

Shader::Shader(const char* vertexShaderFilepath, const char* geometryShaderFilepath, const char* fragmentShaderFilepath)
	: m_ShaderProgramID()
{
	int success;
	char infoLog[512];

	unsigned int vertexShaderID = CreateShader(vertexShaderFilepath, GL_VERTEX_SHADER);
	unsigned int geometryShaderID = CreateShader(geometryShaderFilepath, GL_GEOMETRY_SHADER);
	unsigned int fragmentShaderID = CreateShader(fragmentShaderFilepath, GL_FRAGMENT_SHADER);

	m_ShaderProgramID = glCreateProgram();

	glAttachShader(m_ShaderProgramID, vertexShaderID);
	glAttachShader(m_ShaderProgramID, geometryShaderID);
	glAttachShader(m_ShaderProgramID, fragmentShaderID);

	glLinkProgram(m_ShaderProgramID);
	glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(m_ShaderProgramID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Linkage failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(geometryShaderID);
	glDeleteShader(fragmentShaderID);
}

Shader::~Shader()
{
}

void Shader::Bind()
{
	glUseProgram(m_ShaderProgramID);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const char* uniformName, int data)
{
	int uniformLocation = glGetUniformLocation(m_ShaderProgramID, uniformName);

	if (uniformLocation > -1)
	{
		glUniform1i(uniformLocation, data);
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"." << std::endl;
	}
}

void Shader::SetUniform1f(const char* uniformName, float data)
{
	int uniformLocation = glGetUniformLocation(m_ShaderProgramID, uniformName);

	if (uniformLocation > -1)
	{
		glUniform1f(uniformLocation, data);
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"." << std::endl;
	}
}

void Shader::SetUniform3f(const char* uniformName, const glm::vec3& data)
{
	int uniformLocation = glGetUniformLocation(m_ShaderProgramID, uniformName);

	if (uniformLocation > -1)
	{
		glUniform3f(uniformLocation, data.x, data.y, data.z);
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"." << std::endl;
	}
}

void Shader::SetUniformMatrix4fv(const char* uniformName, const glm::mat4& data)
{
	int uniformLocation = glGetUniformLocation(m_ShaderProgramID, uniformName);

	if (uniformLocation > -1)
	{
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(data));
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"." << std::endl;
	}
}

const unsigned int Shader::CreateShader(const char* filepath, int shaderType)
{
	int success;
	char infoLog[512];

	std::ifstream fileStream(filepath);
	std::stringstream stringStream; stringStream << fileStream.rdbuf();
	std::string shaderSource = stringStream.str();

	const char* shaderCode = shaderSource.c_str();
	unsigned int shaderID = glCreateShader(shaderType);

	glShaderSource(shaderID, 1, &shaderCode, NULL);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Compilation failed!\n" << infoLog << std::endl;

		glDeleteShader(shaderID);

		return -1;
	}

	return shaderID;
}

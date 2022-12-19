#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "../../graphics/core/Shader.h"

class Sun
{
public:
	Sun(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);
	~Sun();

	void SetPosition(const glm::vec3& displacement);

	const glm::vec3& GetPosition();
	const glm::vec3& GetDirection();

	const glm::mat4& GetViewMatrix();

	void Render(Shader* shaderProgram);

private:
	glm::vec3 m_InitialPosition, m_Position, m_Direction, m_Up;
	glm::mat4 m_ViewMatrix;

	unsigned int m_VAO, m_VBO, m_EBO;

	void InitializeRenderData();
};

#include "Sun.h"

Sun::Sun(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up)
	: m_InitialPosition(position), m_Position(position), m_Direction(direction), m_Up(up), m_VAO(), m_VBO(), m_EBO()
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);

	InitializeRenderData();
}

Sun::~Sun()
{
}

void Sun::SetPosition(const glm::vec3& displacement)
{
	m_Position = m_InitialPosition + displacement;

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);
}

const glm::vec3& Sun::GetPosition()
{
	return m_Position;
}

const glm::vec3& Sun::GetDirection()
{
	return m_Direction;
}

const glm::mat4& Sun::GetViewMatrix()
{
	return m_ViewMatrix;
}

void Sun::Render(Shader* shaderProgram)
{
	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, m_Position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f));

	shaderProgram->SetUniformMatrix4fv("uModelMatrix", modelMatrix);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Sun::InitializeRenderData()
{
	float vertices[] = {
	// front
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
	// back
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
	// right
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
	// left
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	// top
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
	// bottom
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f
	};

	unsigned int indices[] = {
		 0,  1,  2,  2,  3,  0,
		 4,  5,  6,  6,  7,  4,
		 8,  9, 10, 10, 11,  8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20,
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#include "HUD.h"

HUD::HUD(unsigned int screenWidth, unsigned int screenHeight)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight),
	  m_VAO(), m_VBO(), m_EBO(),
	  m_HUDRenderShader("res/shaders/ui_render_vs.glsl", "res/shaders/ui_render_fs.glsl"),
	  m_CrossHairTex("res/textures/crosshair.png")
{
	InitializeRenderData();

	glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);

	m_HUDRenderShader.Bind();
	m_HUDRenderShader.SetUniformMatrix4fv("uProjectionMatrix", projectionMatrix);
	m_HUDRenderShader.SetUniform1i("uTexture", 1);
	m_HUDRenderShader.Unbind();

	m_CrossHairTex.Bind(1);
}

HUD::~HUD()
{
}

void HUD::Render()
{
	m_HUDRenderShader.Bind();

	// Draw crosshair.
	{
		glm::vec3 crossHairTexSize = m_CrossHairTex.GetSize();
		glm::mat4 crossHairModelMatrix(1.0f);

		crossHairModelMatrix = glm::translate(crossHairModelMatrix, glm::vec3((m_ScreenWidth / 2.0f) - (crossHairTexSize.x / 2.0f), (m_ScreenHeight / 2.0f) - (crossHairTexSize.y / 2.0f), 0.0f));
		crossHairModelMatrix = glm::scale(crossHairModelMatrix, glm::vec3(20.0f, 20.0f, 1.0f));

		m_HUDRenderShader.SetUniformMatrix4fv("uModelMatrix", crossHairModelMatrix);

		m_CrossHairTex.Bind(1);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	m_HUDRenderShader.Unbind();
}

void HUD::InitializeRenderData()
{
	float vertices[] = {
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2, 2, 3, 0
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#include "DepthMapRenderer.h"

DepthMapRenderer::DepthMapRenderer()
	: m_VAO(), m_VBO(), m_EBO(),
	  m_DepthMapShader("res/shaders/dev/depth_map_render_vs.glsl", "res/shaders/dev/depth_map_render_fs.glsl")
{
	InitializeRenderData();
}

DepthMapRenderer::~DepthMapRenderer()
{
}

void DepthMapRenderer::Render(int textureUnit)
{
	m_DepthMapShader.Bind();

	m_DepthMapShader.SetUniform1i("uDepthMap", textureUnit);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	m_DepthMapShader.Unbind();
}

void DepthMapRenderer::InitializeRenderData()
{
	float vertices[] = { // Inverting the "top" and "bottom" of the texture coordinates is a trick to prevent some shadow map artifacts.
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

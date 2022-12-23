#pragma once

#include <glad/glad.h>

#include "../graphics/core/Shader.h"

class DepthMapRenderer
{
public:
	DepthMapRenderer();
	~DepthMapRenderer();

	void Render(int textureUnit);

private:
	unsigned int m_VAO, m_VBO, m_EBO;

	Shader m_DepthMapShader;

	void InitializeRenderData();
};

#pragma once

#include <glm/glm.hpp>

#include "../graphics/core/Shader.h"
#include "../graphics/core/Texture.h"

class HUD
{
public:
	HUD(unsigned int screenWidth, unsigned int screenHeight);
	~HUD();

	void Render();

private:
	unsigned int m_ScreenWidth, m_ScreenHeight;
	unsigned int m_VAO, m_VBO, m_EBO;

	Shader m_HUDRenderShader;

	Texture m_CrossHairTex;

	void InitializeRenderData();
};

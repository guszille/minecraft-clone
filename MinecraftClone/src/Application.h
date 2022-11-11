#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "graphics/core/Shader.h"
#include "graphics/core/Texture.h"

#include "util/Camera.h"
#include "util/World.h"

class Application
{
public:
	Application(unsigned int screenWidth, unsigned int screenHeight);
	~Application();

	void Setup();
	void Update(float deltaTime);
	void ProcessInput(float deltaTime);
	void Render();

	void SetKeyState(int index, bool pressed);
	void SetMousePosition(float x, float y);

private:
	unsigned int m_ScreenWidth, m_ScreenHeight;
	
	bool m_Keys[1024];
	bool m_CursorAttached;
	glm::vec2 m_LastMousePosition, m_CurrMousePosition;

	glm::mat4 m_ProjectionMatrix;
};

#include "Application.h"

Player* g_Player;
World* g_World;
Shader* g_BlockRenderShader;
Texture* g_SpriteSheet;

float g_CameraSensitivity = 0.05f;

Application::Application(unsigned int screenWidth, unsigned int screenHeight)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight), m_Keys(), m_MouseButtons(), m_MouseButtonsProcessed(), m_CursorAttached(false), m_LastMousePosition(), m_CurrMousePosition()
{
	float fieldOfView = 45.0f;
	float aspectRatio = (float)screenWidth / (float)screenHeight;

	m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 1000.0f);
}

Application::~Application()
{
	delete g_Player;
	delete g_World;
	delete g_BlockRenderShader;
	delete g_SpriteSheet;
}

void Application::Setup()
{
	g_Player = new Player(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), 15.0f, 4.0f);
	g_World = new World();
	g_BlockRenderShader = new Shader("res/shaders/block_render_vs.glsl", "res/shaders/block_render_fs.glsl");
	g_SpriteSheet = new Texture("res/textures/minecraft_spritesheet.png");

	WorldGenerator::Execute(g_World, std::pair<int, int>(0, 0), 8);

	g_BlockRenderShader->Bind();
	g_BlockRenderShader->SetUniformMatrix4fv("uProjectionMatrix", m_ProjectionMatrix);
	g_BlockRenderShader->SetUniform1i("uTexture", 0);
	g_BlockRenderShader->Unbind();

	g_SpriteSheet->Bind(0);
}

void Application::Update(float deltaTime)
{
}

void Application::ProcessInput(float deltaTime)
{
	if (m_Keys[GLFW_KEY_W]) { g_Player->SetPosition(Camera::Direction::FORWARD, deltaTime); }

	if (m_Keys[GLFW_KEY_S]) { g_Player->SetPosition(Camera::Direction::BACKWARD, deltaTime); }

	if (m_Keys[GLFW_KEY_D]) { g_Player->SetPosition(Camera::Direction::RIGHT, deltaTime); }

	if (m_Keys[GLFW_KEY_A]) { g_Player->SetPosition(Camera::Direction::LEFT, deltaTime); }

	if (m_Keys[GLFW_KEY_LEFT_SHIFT]) { g_Player->SetPosition(Camera::Direction::UP, deltaTime); }

	if (m_Keys[GLFW_KEY_LEFT_CONTROL]) { g_Player->SetPosition(Camera::Direction::DOWN, deltaTime); }

	if (m_LastMousePosition != m_CurrMousePosition)
	{
		if (!m_CursorAttached)
		{
			m_CurrMousePosition = m_LastMousePosition;
			m_CursorAttached = true;
		}

		float xOffset = (m_LastMousePosition.x - m_CurrMousePosition.x) * g_CameraSensitivity;
		float yOffset = (m_CurrMousePosition.y - m_LastMousePosition.y) * g_CameraSensitivity;

		m_CurrMousePosition = m_LastMousePosition;

		g_Player->SetDirection(xOffset, yOffset);
	}

	if (m_MouseButtons[GLFW_MOUSE_BUTTON_RIGHT] && !m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_RIGHT])
	{
		g_World->CastRay(g_Player->GetPosition(), g_Player->GetDirection(), g_Player->m_Range);

		m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_RIGHT] = true;
	}
}

void Application::Render()
{
	g_BlockRenderShader->Bind();
	g_BlockRenderShader->SetUniformMatrix4fv("uViewMatrix", g_Player->GetViewMatrix());

	g_World->Render(g_BlockRenderShader);

	g_BlockRenderShader->Unbind();
}

void Application::SetKeyState(int index, bool pressed)
{
	m_Keys[index] = pressed;
}

void Application::SetMouseButtonState(int index, bool pressed)
{
	m_MouseButtons[index] = pressed;
	m_MouseButtonsProcessed[index] = false;
}

void Application::SetMousePosition(float x, float y)
{
	m_LastMousePosition = glm::vec2(x, y);
}

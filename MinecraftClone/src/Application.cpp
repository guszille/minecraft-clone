#include "Application.h"

Camera* g_Camera;
World* g_World;
Shader* g_BlockRenderShader;
Texture* g_SpriteSheet;

float g_CameraSpeed = 10.0f;
float g_CameraSensitivity = 0.05f;

Application::Application(unsigned int screenWidth, unsigned int screenHeight)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight), m_Keys(), m_CursorAttached(false), m_LastMousePosition(), m_CurrMousePosition()
{
	float fieldOfView = 45.0f;
	float aspectRatio = (float)screenWidth / (float)screenHeight;

	m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, 0.1f, 100.0f);
}

Application::~Application()
{
	delete g_Camera;
	delete g_World;
	delete g_BlockRenderShader;
	delete g_SpriteSheet;
}

void Application::Setup()
{
	g_Camera = new Camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	g_World = new World();
	g_BlockRenderShader = new Shader("res/shaders/block_render_vs.glsl", "res/shaders/block_render_fs.glsl");
	g_SpriteSheet = new Texture("res/textures/minecraft_spritesheet.png");

	// World chunk/blocks generation.
	//
	for (int i = -8; i < 8; i++)
	{
		for (int j = -8; j < 8; j++)
		{
			std::pair<int, int> chunkPosition = { i, j };
			Chunk chunk(chunkPosition);
	
			for (unsigned int x = 0; x < Chunk::s_DefaultDimensions.x; x++)
			{
				for (unsigned int y = 0; y < Chunk::s_DefaultDimensions.y / 2; y++)
				{
					for (unsigned int z = 0; z < Chunk::s_DefaultDimensions.z; z++)
					{
						chunk.InsertBlock(Block(Block::Type::GRASS, glm::ivec3(x, y, z)));
					}
				}
			}

			g_World->InsertChunk(chunk);
		}
	}

	g_World->GenerateMeshes();

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
	if (m_Keys[GLFW_KEY_W])
	{
		g_Camera->SetPosition(Camera::Direction::FORWARD, g_CameraSpeed * deltaTime);
	}

	if (m_Keys[GLFW_KEY_S])
	{
		g_Camera->SetPosition(Camera::Direction::BACKWARD, g_CameraSpeed * deltaTime);
	}

	if (m_Keys[GLFW_KEY_D])
	{
		g_Camera->SetPosition(Camera::Direction::RIGHT, g_CameraSpeed * deltaTime);
	}

	if (m_Keys[GLFW_KEY_A])
	{
		g_Camera->SetPosition(Camera::Direction::LEFT, g_CameraSpeed * deltaTime);
	}

	if (m_Keys[GLFW_KEY_LEFT_SHIFT])
	{
		g_Camera->SetPosition(Camera::Direction::UP, g_CameraSpeed * deltaTime);
	}

	if (m_Keys[GLFW_KEY_LEFT_CONTROL])
	{
		g_Camera->SetPosition(Camera::Direction::DOWN, g_CameraSpeed * deltaTime);
	}

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

		g_Camera->SetDirection(xOffset, yOffset);
	}
}

void Application::Render()
{
	g_BlockRenderShader->Bind();
	g_BlockRenderShader->SetUniformMatrix4fv("uViewMatrix", g_Camera->GetViewMatrix());

	g_World->Render(g_BlockRenderShader);

	g_BlockRenderShader->Unbind();
}

void Application::SetKeyState(int index, bool pressed)
{
	m_Keys[index] = pressed;
}

void Application::SetMousePosition(float x, float y)
{
	m_LastMousePosition = glm::vec2(x, y);
}

#include "Application.h"

Player* g_Player;
World* g_World;
Shader* g_BlockRenderShader;
Texture* g_SpriteSheet;

float g_CameraSensitivity = 0.05f;

Application::Application(unsigned int screenWidth, unsigned int screenHeight)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight), m_Keys(), m_MouseButtons(), m_MouseButtonsProcessed(), m_CursorAttached(false), m_LastMousePosition(), m_CurrMousePosition()
{
	std::srand(std::time(nullptr)); // Use current time as seed for random generator.

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
	g_Player = new Player(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), 15.0f, 5.0f);
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

	if (m_MouseButtons[GLFW_MOUSE_BUTTON_LEFT] && !m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_LEFT])
	{
		Intersection i = g_World->CastRay(g_Player->GetPosition(), g_Player->GetDirection(), g_Player->m_Range);

		if (std::get<0>(i))
		{
			std::pair<int, int> chunkPosition = std::get<1>(i);
			glm::ivec3 localBlockPosition = std::get<2>(i);
			glm::vec3 worldBlockPosition = std::get<3>(i);
			int intersectedFace = std::get<4>(i);
			glm::ivec3 intersectedFaceNormal = Block::s_CubeNormals[intersectedFace];

			std::pair<int, int> newChunkPosition = chunkPosition;
			glm::ivec3 newLocalBlockPosition = Chunk::GetNextLocalBlockPosition(localBlockPosition, intersectedFaceNormal);
			glm::vec3 newWorldBlockPosition = worldBlockPosition + (glm::vec3)intersectedFaceNormal;

			if (!Chunk::IsAValidPosition(localBlockPosition + Block::s_CubeNormals[intersectedFace]))
			{
				newChunkPosition = { chunkPosition.first + intersectedFaceNormal.x, chunkPosition.second + intersectedFaceNormal.z };
			}

			g_World->InsertBlockAt(newChunkPosition, Block(Block::Type::DIRTY, newWorldBlockPosition), newLocalBlockPosition);
			g_World->UpdateChunkMesh(newChunkPosition);

			if (newLocalBlockPosition.x == 0)
			{
				g_World->UpdateChunkMesh({ newChunkPosition.first - 1, newChunkPosition.second });
			}
			else if (newLocalBlockPosition.x == Chunk::s_DefaultDimensions.x - 1)
			{
				g_World->UpdateChunkMesh({ newChunkPosition.first + 1, newChunkPosition.second });
			}

			if (newLocalBlockPosition.z == 0)
			{
				g_World->UpdateChunkMesh({ newChunkPosition.first, newChunkPosition.second - 1 });
			}
			else if (newLocalBlockPosition.z == Chunk::s_DefaultDimensions.z - 1)
			{
				g_World->UpdateChunkMesh({ newChunkPosition.first, newChunkPosition.second + 1 });
			}

			// FIXME: Debug only.
			{
				std::cout << "BLOCK PLACED AT: (" << newWorldBlockPosition.x << ", " << newWorldBlockPosition.y << ", " << newWorldBlockPosition.z << ")" << std::endl;
				std::cout << "INTERSECTED FACE: " << intersectedFace << std::endl;
			}
		}

		m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_LEFT] = true;
	}

	if (m_MouseButtons[GLFW_MOUSE_BUTTON_RIGHT] && !m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_RIGHT])
	{
		Intersection i = g_World->CastRay(g_Player->GetPosition(), g_Player->GetDirection(), g_Player->m_Range);

		if (std::get<0>(i))
		{
			std::pair<int, int> chunkPosition = std::get<1>(i);
			glm::ivec3 localBlockPosition = std::get<2>(i);

			g_World->RemoveBlockAt(chunkPosition, localBlockPosition);
			g_World->UpdateChunkMesh(chunkPosition);

			if (localBlockPosition.x == 0)
			{
				g_World->UpdateChunkMesh({ chunkPosition.first - 1, chunkPosition.second });
			}
			else if (localBlockPosition.x == Chunk::s_DefaultDimensions.x - 1)
			{
				g_World->UpdateChunkMesh({ chunkPosition.first + 1, chunkPosition.second });
			}

			if (localBlockPosition.z == 0)
			{
				g_World->UpdateChunkMesh({ chunkPosition.first, chunkPosition.second - 1 });
			}
			else if (localBlockPosition.z == Chunk::s_DefaultDimensions.z - 1)
			{
				g_World->UpdateChunkMesh({ chunkPosition.first, chunkPosition.second + 1 });
			}

			// FIXME: Debug only.
			{
				glm::vec3 worldBlockPosition = std::get<3>(i);
				int intersectedFace = std::get<4>(i);

				std::cout << "BLOCK REMOVED AT: (" << worldBlockPosition.x << ", " << worldBlockPosition.y << ", " << worldBlockPosition.z << ")" << std::endl;
				std::cout << "INTERSECTED FACE: " << intersectedFace << std::endl;
			}
		}

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

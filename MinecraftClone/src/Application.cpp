#include "Application.h"

Player* g_Player;
World* g_World;
Shader* g_ChunkMeshRenderShader;
Shader* g_PlayerRenderShader;
Texture* g_SpriteSheet;
HUD* g_HUD;
TextRenderer* g_TextRenderer;

float g_NearPlane = 0.1f;
float g_FarPlane = 1000.0f;
float g_CameraSensitivity = 0.05f;

int g_RenderDistance = 2; // In chunks.
int g_InitialRenderDistance = 4 * g_RenderDistance;

bool g_PhysicsEnabled = false;

float g_Gravity = -75.0f;
float g_Friction = 150.0f;

Application::Application(unsigned int screenWidth, unsigned int screenHeight)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight),
	  m_Keys(), m_KeysProcessed(), m_MouseButtons(), m_MouseButtonsProcessed(), m_CursorAttached(false), m_LastMousePosition(), m_CurrMousePosition(),
	  m_GameMode(GameMode::SURVIVAL)
{
	std::srand((unsigned int)std::time(nullptr)); // Use current time as seed for random generator.

	float fieldOfView = 45.0f;
	float aspectRatio = (float)screenWidth / (float)screenHeight;

	m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, g_NearPlane, g_FarPlane);
}

Application::~Application()
{
	delete g_Player;
	delete g_World;
	delete g_ChunkMeshRenderShader;
	delete g_PlayerRenderShader;
	delete g_SpriteSheet;
	delete g_HUD;
	delete g_TextRenderer;
}

void Application::Setup()
{
	g_Player = new Player(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), 10.0f, 15.0f, 5.0f);
	g_World = new World(std::rand());
	g_ChunkMeshRenderShader = new Shader("res/shaders/chunk_mesh_render_vs.glsl", "res/shaders/chunk_mesh_render_fs.glsl");
	g_PlayerRenderShader = new Shader("res/shaders/player_render_vs.glsl", "res/shaders/player_render_fs.glsl");
	g_SpriteSheet = new Texture("res/textures/minecraft_spritesheet.png");
	g_HUD = new HUD(m_ScreenWidth, m_ScreenHeight);
	g_TextRenderer = new TextRenderer(m_ScreenWidth, m_ScreenHeight);

	NoiseGenerator::Configure(g_World->GetSeed());

	g_World->Setup(std::pair<int, int>(0, 0), g_InitialRenderDistance);

	g_ChunkMeshRenderShader->Bind();
	g_ChunkMeshRenderShader->SetUniformMatrix4fv("uProjectionMatrix", m_ProjectionMatrix);
	g_ChunkMeshRenderShader->SetUniform1i("uTexture", 0);
	g_ChunkMeshRenderShader->Unbind();

	g_PlayerRenderShader->Bind();
	g_PlayerRenderShader->SetUniformMatrix4fv("uProjectionMatrix", m_ProjectionMatrix);
	g_PlayerRenderShader->Unbind();

	g_SpriteSheet->Bind(0); // Keep the main atlas texture on 0 unit position.

	// Default OpenGL configs.
	{
		// GL_DEPTH_TEST
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		// GL_CULL_FACE
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		// GL_BLEND
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void Application::Update(float deltaTime)
{
	std::pair<int, int> origin = Chunk::GetChunkPositionFromWorld(g_Player->GetPosition());

	g_World->Update(origin, g_RenderDistance, deltaTime);

	if (g_PhysicsEnabled)
	{
		g_Player->Update(g_World, g_Gravity, g_Friction, deltaTime);
	}
}

void Application::ProcessInput(float deltaTime)
{
	if (m_Keys[GLFW_KEY_F1])
	{
		g_PhysicsEnabled = true;
	}

	if (m_Keys[GLFW_KEY_W] || m_Keys[GLFW_KEY_S] || m_Keys[GLFW_KEY_D] || m_Keys[GLFW_KEY_A])
	{
		if (m_Keys[GLFW_KEY_W]) { g_Player->ApplyHorizontalVelocity(Direction::FORWARD); }
		if (m_Keys[GLFW_KEY_S]) { g_Player->ApplyHorizontalVelocity(Direction::BACKWARD); }
		if (m_Keys[GLFW_KEY_D]) { g_Player->ApplyHorizontalVelocity(Direction::RIGHT); }
		if (m_Keys[GLFW_KEY_A]) { g_Player->ApplyHorizontalVelocity(Direction::LEFT); }

		g_Player->ResolveHorizontalMovement();
	}

	if (m_Keys[GLFW_KEY_SPACE] && !m_KeysProcessed[GLFW_KEY_SPACE])
	{
		if (g_PhysicsEnabled)
		{
			g_Player->ApplyVerticalVelocity(Direction::UP);
		}

		m_KeysProcessed[GLFW_KEY_SPACE] = true;
	}

	if (m_LastMousePosition != m_CurrMousePosition)
	{
		if (!m_CursorAttached)
		{
			m_CurrMousePosition = m_LastMousePosition;
			m_CursorAttached = true;
		}

		float xRotationOffset = (m_LastMousePosition.x - m_CurrMousePosition.x) * g_CameraSensitivity;
		float yRotationOffset = (m_CurrMousePosition.y - m_LastMousePosition.y) * g_CameraSensitivity;

		m_CurrMousePosition = m_LastMousePosition;

		g_Player->CalculateViewDirection(xRotationOffset, yRotationOffset);
	}

	if (m_MouseButtons[GLFW_MOUSE_BUTTON_LEFT] && !m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_LEFT])
	{
		const glm::vec3& playerPosition = g_Player->GetPosition();
		const glm::vec3& playerDirection = g_Player->GetViewDirection();

		Intersection i = g_World->CastRay(playerPosition + (playerDirection * g_NearPlane), playerDirection, g_Player->m_Range);

		if (std::get<0>(i))
		{
			std::pair<int, int> chunkPosition = std::get<1>(i);
			glm::ivec3 localBlockPosition = std::get<2>(i);
			glm::vec3 globalBlockPosition = std::get<3>(i);
			int intersectedFace = std::get<4>(i);
			glm::ivec3 intersectedFaceNormal = Cube::s_Normals[intersectedFace];

			std::pair<int, int> newChunkPosition = chunkPosition;
			glm::ivec3 newLocalBlockPosition = Chunk::GetNextLocalBlockPosition(localBlockPosition, intersectedFaceNormal);
			glm::vec3 newWorldBlockPosition = globalBlockPosition + (glm::vec3)intersectedFaceNormal;

			if (!Chunk::IsAValidPosition(localBlockPosition + Cube::s_Normals[intersectedFace]))
			{
				newChunkPosition = { chunkPosition.first + intersectedFaceNormal.x, chunkPosition.second + intersectedFaceNormal.z };
			}

			if (g_World->InsertBlockAt(newChunkPosition, Block(BlockType::DIRTY, newWorldBlockPosition), newLocalBlockPosition))
			{
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
			}
		}

		m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_LEFT] = true;
	}

	if (m_MouseButtons[GLFW_MOUSE_BUTTON_RIGHT] && !m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_RIGHT])
	{
		const glm::vec3& playerPosition = g_Player->GetPosition();
		const glm::vec3& playerDirection = g_Player->GetViewDirection();

		Intersection i = g_World->CastRay(playerPosition + (playerDirection * g_NearPlane), playerDirection, g_Player->m_Range);

		if (std::get<0>(i))
		{
			std::pair<int, int> chunkPosition = std::get<1>(i);
			glm::ivec3 localBlockPosition = std::get<2>(i);

			if (g_World->RemoveBlockAt(chunkPosition, localBlockPosition))
			{
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
			}
		}

		m_MouseButtonsProcessed[GLFW_MOUSE_BUTTON_RIGHT] = true;
	}
}

void Application::Render()
{
	// Render the game.
	{
		g_ChunkMeshRenderShader->Bind();
		g_ChunkMeshRenderShader->SetUniformMatrix4fv("uViewMatrix", g_Player->GetViewMatrix());
		g_World->Render(g_ChunkMeshRenderShader);
		g_ChunkMeshRenderShader->Unbind();

		// g_Player->Render(g_PlayerRenderShader);
	}

	// Render the UI.
	{
		g_HUD->Render();
	
		// Render any text.
		{
			const glm::vec3& playerPosition = g_Player->GetPosition();
			const glm::vec3& playerDirection = g_Player->GetViewDirection();

			std::string debugText1 = "POSITION: (" + std::to_string(playerPosition.x) + ", " + std::to_string(playerPosition.y) + ", " + std::to_string(playerPosition.z) + ")";
			std::string debugText2 = "DIRECTION: (" + std::to_string(playerDirection.x) + ", " + std::to_string(playerDirection.y) + ", " + std::to_string(playerDirection.z) + ")";

			g_TextRenderer->Write(debugText1, 16.0f, 16.0f, 1.0f, glm::vec3(1.0f));
			g_TextRenderer->Write(debugText2, 16.0f, 32.0f, 1.0f, glm::vec3(1.0f));
		}
	}
}

void Application::SetKeyState(int index, bool pressed)
{
	m_Keys[index] = pressed;
	m_KeysProcessed[index] = false;
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

#include "Application.h"

Player* g_Player;
InventorySystem* g_Inventory;

World* g_World;

Sun* g_Sun;

HUD* g_HUD;

Shader* g_ShadowMapRenderShader;
Shader* g_ChunkMeshRenderShader;
Shader* g_PlayerRenderShader;
Shader* g_CubeRenderShader;
Shader* g_OcclusionMapRenderShader; // DEBUG.

Texture* g_SpriteSheet;

DepthMap* g_ShadowMap;

TextRenderer* g_TextRenderer;

DepthMapRenderer* g_DepthMapRenderer; // DEBUG.

glm::vec3 g_ClearColor(0.45f, 0.65f, 1.0f);

float g_NearPlane = 0.1f;
float g_FarPlane = 1024.0f;
float g_CameraSensitivity = 0.05f;

float g_ShadowMapNearPlane = 0.1f;
float g_ShadowMapFarPlane = 512.0f;
int   g_ShadowMapWidth = 16384;
int   g_ShadowMapHeight = 16384;
float g_MinimumShadowBias = 0.00005f; // To prevent shadow acne.
bool  g_RenderDepthMap = false;
bool  g_RenderOcclusionMap = false;

int   g_RenderDistance = 16; // In chunks.
int   g_InitialRenderDistance = g_RenderDistance;
int   g_ViewDistance = 16 * g_RenderDistance; // In blocks.

float g_FogRadius = (float)(g_ViewDistance - 32);
float g_FogDensity = 0.05f;

float g_Gravity = -75.0f;
float g_Friction = 150.0f;

Application::Application(unsigned int screenWidth, unsigned int screenHeight)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight),
	  m_Keys(), m_KeysProcessed(), m_MouseButtons(), m_MouseButtonsProcessed(), m_CursorAttached(false), m_LastMousePosition(), m_CurrMousePosition(),
	  m_GameMode(GameMode::DEBUG)
{
	std::srand((unsigned int)std::time(nullptr)); // Use current time as seed for random generator.

	float fieldOfView = 60.0f;
	float aspectRatio = (float)screenWidth / (float)screenHeight;
	float halfEdge = (float)g_ViewDistance;

	m_CameraProjectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, g_NearPlane, g_FarPlane);
	m_ShadowMapProjectionMatrix = glm::ortho(-halfEdge, halfEdge, halfEdge, -halfEdge, g_ShadowMapNearPlane, g_ShadowMapFarPlane); // Inverting the "top" and "bottom" of orthographic projection matrix is a trick to prevent some shadow map artifacts.
}

Application::~Application()
{
	delete g_Player;
	delete g_Inventory;
	delete g_World;
	delete g_HUD;
	delete g_ShadowMapRenderShader;
	delete g_ChunkMeshRenderShader;
	delete g_PlayerRenderShader;
	delete g_CubeRenderShader;
	delete g_OcclusionMapRenderShader;
	delete g_SpriteSheet;
	delete g_ShadowMap;
	delete g_TextRenderer;
	delete g_DepthMapRenderer;
}

void Application::Setup()
{
	g_Player = new Player(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), 10.0f, 15.0f, 10.0f);
	g_Inventory = new InventorySystem();
	g_World = new World(std::rand(), 0.005f, 0.035f, 0.01f, 0.0025f);
	g_Sun = new Sun(glm::vec3(192.0f, 192.0f, 0.0f), glm::vec3(-192.0f, -192.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	g_HUD = new HUD(m_ScreenWidth, m_ScreenHeight);
	g_ShadowMapRenderShader = new Shader("res/shaders/shadow_map_render_vs.glsl", "res/shaders/shadow_map_render_fs.glsl");
	g_ChunkMeshRenderShader = new Shader("res/shaders/chunk_mesh_render_vs.glsl", "res/shaders/chunk_mesh_render_fs.glsl");
	g_PlayerRenderShader = new Shader("res/shaders/player_render_vs.glsl", "res/shaders/player_render_fs.glsl");
	g_CubeRenderShader = new Shader("res/shaders/cube_render_vs.glsl", "res/shaders/cube_render_fs.glsl");
	g_OcclusionMapRenderShader = new Shader("res/shaders/dev/occlusion_map_render_vs.glsl", "res/shaders/dev/occlusion_map_render_fs.glsl");
	g_SpriteSheet = new Texture("res/textures/minecraft_spritesheet.png");
	g_ShadowMap = new DepthMap(g_ShadowMapWidth, g_ShadowMapHeight);
	g_TextRenderer = new TextRenderer(m_ScreenWidth, m_ScreenHeight);
	g_DepthMapRenderer = new DepthMapRenderer();

	g_Inventory->InsertBlockAtEnd(Block(BlockType::GRASS, glm::vec3(0.0f)), "Grass");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::DIRTY, glm::vec3(0.0f)), "Dirty");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::STONE, glm::vec3(0.0f)), "Stone");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::BRICKS, glm::vec3(0.0f)), "Bricks");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::SAND, glm::vec3(0.0f)), "Sand");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::BIRCHWOODPLANK, glm::vec3(0.0f)), "Birch Wood Plank");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::OAKWOOKPLANK, glm::vec3(0.0f)), "Oak Wood Plank");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::DARKOAKWOOKPLANK, glm::vec3(0.0f)), "Dark Oak Wood Plank");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::ACACIAWOODPLANK, glm::vec3(0.0f)), "Acacia Wood Plank");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::GRASSLEAVES, glm::vec3(0.0f), false, true, true), "Grass Leaves");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::ORANGETULIP, glm::vec3(0.0f), false, true, true), "Orange Tulip");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::PINKTULIP, glm::vec3(0.0f), false, true, true), "Pink Tulip");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::REDTULIP, glm::vec3(0.0f), false, true, true), "Red Tulip");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::POPPY, glm::vec3(0.0f), false, true, true), "Poppy");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::SUNFLOWER, glm::vec3(0.0f), false, true, true), "Sunflower");
	g_Inventory->InsertBlockAtEnd(Block(BlockType::GLASS, glm::vec3(0.0f), true, true), "Glass");

	g_World->Setup(std::pair<int, int>(0, 0), g_InitialRenderDistance);

	g_ShadowMapRenderShader->Bind();
	g_ShadowMapRenderShader->SetUniform1f("uAlphaThreshold", 0.95f);
	g_ShadowMapRenderShader->SetUniform1i("uTexture", 0);
	g_ShadowMapRenderShader->Unbind();

	g_ChunkMeshRenderShader->Bind();
	g_ChunkMeshRenderShader->SetUniformMatrix4fv("uProjectionMatrix", m_CameraProjectionMatrix);
	g_ChunkMeshRenderShader->SetUniform3f("uLight.Ambient", glm::vec3(0.35f, 0.35f, 0.35f));
	g_ChunkMeshRenderShader->SetUniform3f("uLight.Diffuse", glm::vec3(0.85f, 0.85f, 0.85f));
	g_ChunkMeshRenderShader->SetUniform3f("uLight.Specular", glm::vec3(0.0f, 0.0f, 0.0f)); // No specular component.
	g_ChunkMeshRenderShader->SetUniform3f("uLight.Position", g_Sun->GetPosition());
	g_ChunkMeshRenderShader->SetUniform3f("uLight.Direction", g_Sun->GetDirection());
	g_ChunkMeshRenderShader->SetUniform1i("uMaterial.Diffuse", 0);
	g_ChunkMeshRenderShader->SetUniform1i("uMaterial.Specular", 0);
	g_ChunkMeshRenderShader->SetUniform1f("uMaterial.Shininess", 64.0f);
	g_ChunkMeshRenderShader->SetUniform1i("uShadowMap", 1);
	g_ChunkMeshRenderShader->SetUniform1i("uShaded", 1);
	g_ChunkMeshRenderShader->SetUniform1i("uOccluded", 1);
	g_ChunkMeshRenderShader->SetUniform1i("uObscured", 1);
	g_ChunkMeshRenderShader->SetUniform1f("uAlphaThreshold", 0.05f);
	g_ChunkMeshRenderShader->SetUniform1f("uFogRadius", g_FogRadius);
	g_ChunkMeshRenderShader->SetUniform1f("uFogDensity", g_FogDensity);
	g_ChunkMeshRenderShader->SetUniform3f("uFogColor", g_ClearColor); // Same as the clear color or skybox texture.
	g_ChunkMeshRenderShader->SetUniform1i("uAiming", 0);
	g_ChunkMeshRenderShader->Unbind();

	g_PlayerRenderShader->Bind();
	g_PlayerRenderShader->SetUniformMatrix4fv("uProjectionMatrix", m_CameraProjectionMatrix);
	g_PlayerRenderShader->Unbind();

	g_CubeRenderShader->Bind();
	g_CubeRenderShader->SetUniformMatrix4fv("uProjectionMatrix", m_CameraProjectionMatrix);
	g_CubeRenderShader->Unbind();

	g_OcclusionMapRenderShader->Bind();
	g_OcclusionMapRenderShader->SetUniformMatrix4fv("uProjectionMatrix", m_CameraProjectionMatrix);
	g_OcclusionMapRenderShader->Unbind();

	g_SpriteSheet->Bind(0); // Keep the main atlas texture on 0 unit position.
	g_ShadowMap->BindDepthBuffer(1); // Keep the shadow map texture on 1 unit position.

	// Default OpenGL configs.
	{
		// GL_MULTISAMPLE
		// 
		// WARNING: Multisample can cause some texture artifacts when combined with GL_NEAREST filtering.
		// 
		// glEnable(GL_MULTISAMPLE);

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
	const glm::vec3& playerPosition = g_Player->GetPosition();
	const glm::vec3& playerDirection = g_Player->GetViewDirection();
	std::pair<int, int> origin = Chunk::GetChunkPositionFromWorld(playerPosition);

	g_World->Update(origin, g_RenderDistance, deltaTime);

	if (m_GameMode == GameMode::SURVIVAL)
	{
		g_Player->Update(g_World, g_Gravity, g_Friction, deltaTime);
	}

	g_Sun->SetPosition(glm::vec3(playerPosition.x, 0.0f, playerPosition.z)); // Following the player.

	Intersection i = g_World->CastRay(playerPosition + (playerDirection * g_NearPlane), playerDirection, g_Player->m_Range);

	if (std::get<0>(i))
	{
		glm::fvec3 globalBlockPosition = std::get<3>(i);

		g_ChunkMeshRenderShader->Bind();
		g_ChunkMeshRenderShader->SetUniform1i("uAiming", 1);
		g_ChunkMeshRenderShader->SetUniform3f("uAimedBlockPosition", globalBlockPosition);
		g_ChunkMeshRenderShader->Unbind();
	}
	else
	{
		g_ChunkMeshRenderShader->Bind();
		g_ChunkMeshRenderShader->SetUniform1i("uAiming", 0);
		g_ChunkMeshRenderShader->Unbind();
	}
}

void Application::ProcessInput(float deltaTime)
{
	if (m_GameMode == GameMode::SURVIVAL)
	{
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
			g_Player->ApplyVerticalVelocity(Direction::UP);

			m_KeysProcessed[GLFW_KEY_SPACE] = true;
		}
	}
	else if (m_GameMode == GameMode::DEBUG)
	{
		if (m_Keys[GLFW_KEY_W]) { g_Player->CalculatePosition(Direction::FORWARD, deltaTime); }
		if (m_Keys[GLFW_KEY_S]) { g_Player->CalculatePosition(Direction::BACKWARD, deltaTime); }
		if (m_Keys[GLFW_KEY_D]) { g_Player->CalculatePosition(Direction::RIGHT, deltaTime); }
		if (m_Keys[GLFW_KEY_A]) { g_Player->CalculatePosition(Direction::LEFT, deltaTime); }

		if (m_Keys[GLFW_KEY_SPACE]) { g_Player->CalculatePosition(Direction::UP, deltaTime); }
		if (m_Keys[GLFW_KEY_LEFT_CONTROL]) { g_Player->CalculatePosition(Direction::DOWN, deltaTime); }
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

	if (m_Keys[GLFW_KEY_LEFT] && !m_KeysProcessed[GLFW_KEY_LEFT])
	{
		g_Inventory->SelectPreviousSlot();

		m_KeysProcessed[GLFW_KEY_LEFT] = true;
	}

	if (m_Keys[GLFW_KEY_RIGHT] && !m_KeysProcessed[GLFW_KEY_RIGHT])
	{
		g_Inventory->SelectNextSlot();

		m_KeysProcessed[GLFW_KEY_RIGHT] = true;
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
			glm::fvec3 globalBlockPosition = std::get<3>(i);
			int intersectedFace = std::get<4>(i);
			glm::ivec3 intersectedFaceNormal = Cube::s_Normals[intersectedFace];

			std::pair<int, int> newChunkPosition = chunkPosition;
			glm::ivec3 newLocalBlockPosition = Chunk::GetNextLocalBlockPosition(localBlockPosition, intersectedFaceNormal);
			glm::fvec3 newGlobalBlockPosition = globalBlockPosition + (glm::vec3)intersectedFaceNormal;

			if (!Chunk::IsAValidPosition(localBlockPosition + Cube::s_Normals[intersectedFace]))
			{
				newChunkPosition = { chunkPosition.first + intersectedFaceNormal.x, chunkPosition.second + intersectedFaceNormal.z };
			}

			Block newBlock = g_Inventory->GetBlockOnSelectedSlot();
			newBlock.m_Position = newGlobalBlockPosition;

			if (g_World->InsertBlockAt(newChunkPosition, newBlock, newLocalBlockPosition))
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

	// DEBUG INPUTS.
	if (m_Keys[GLFW_KEY_F1] && !m_KeysProcessed[GLFW_KEY_F1])
	{
		m_GameMode = GameMode::SURVIVAL;

		m_KeysProcessed[GLFW_KEY_F1] = true;
	}

	if (m_Keys[GLFW_KEY_F2] && !m_KeysProcessed[GLFW_KEY_F2])
	{
		m_GameMode = GameMode::DEBUG;

		m_KeysProcessed[GLFW_KEY_F2] = true;
	}

	if (m_Keys[GLFW_KEY_F5] && !m_KeysProcessed[GLFW_KEY_F5])
	{
		g_RenderDepthMap = !g_RenderDepthMap;

		m_KeysProcessed[GLFW_KEY_F5] = true;
	}

	if (m_Keys[GLFW_KEY_F6] && !m_KeysProcessed[GLFW_KEY_F6])
	{
		g_RenderOcclusionMap = !g_RenderOcclusionMap;

		m_KeysProcessed[GLFW_KEY_F6] = true;
	}

	if (m_Keys[GLFW_KEY_KP_SUBTRACT] && !m_KeysProcessed[GLFW_KEY_KP_SUBTRACT])
	{
		g_MinimumShadowBias -= 0.00001f;

		m_KeysProcessed[GLFW_KEY_KP_SUBTRACT] = true;
	}

	if (m_Keys[GLFW_KEY_KP_ADD] && !m_KeysProcessed[GLFW_KEY_KP_ADD])
	{
		g_MinimumShadowBias += 0.00001f;

		m_KeysProcessed[GLFW_KEY_KP_ADD] = true;
	}
}

void Application::Render()
{
	glm::mat4 lightSpaceMatrix = m_ShadowMapProjectionMatrix * g_Sun->GetViewMatrix();

	// Render the depth/shadow map.
	{
		glViewport(0, 0, g_ShadowMapWidth, g_ShadowMapHeight);

		glEnable(GL_DEPTH_TEST);

		g_ShadowMap->Bind();
		g_ShadowMapRenderShader->Bind();
		g_ShadowMapRenderShader->SetUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);

		glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		
		glCullFace(GL_FRONT);

		g_World->Render(g_ShadowMapRenderShader, MeshType::OPAQUE);

		glCullFace(GL_BACK);

		glDisable(GL_CULL_FACE);

		g_World->Render(g_ShadowMapRenderShader, MeshType::TRANSLUCENT);

		g_ShadowMapRenderShader->Unbind();
		g_ShadowMap->Unbind();

		glDisable(GL_DEPTH_TEST);
	}

	if (g_RenderDepthMap) // DEBUG: Render the depth map.
	{
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight); // Reset viewport.

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		g_DepthMapRenderer->Render(1);

		glDisable(GL_DEPTH_TEST);
	}
	else if (g_RenderOcclusionMap) // DEBUG: Render occlusion map.
	{
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight); // Reset viewport.

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glClearColor(g_ClearColor.x, g_ClearColor.y, g_ClearColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		g_OcclusionMapRenderShader->Bind();
		g_OcclusionMapRenderShader->SetUniformMatrix4fv("uViewMatrix", g_Player->GetViewMatrix());

		g_World->Render(g_OcclusionMapRenderShader, MeshType::OPAQUE);

		g_OcclusionMapRenderShader->Unbind();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
	}
	else
	{
		// Render the real scene.
		{
			glViewport(0, 0, m_ScreenWidth, m_ScreenHeight); // Reset viewport.

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);

			glClearColor(g_ClearColor.x, g_ClearColor.y, g_ClearColor.z, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g_ChunkMeshRenderShader->Bind();
			g_ChunkMeshRenderShader->SetUniformMatrix4fv("uViewMatrix", g_Player->GetViewMatrix());
			g_ChunkMeshRenderShader->SetUniform3f("uViewPos", g_Player->GetPosition());
			g_ChunkMeshRenderShader->SetUniformMatrix4fv("uLightSpaceMatrix", lightSpaceMatrix);
			g_ChunkMeshRenderShader->SetUniform1f("uMinimumShadowBias", g_MinimumShadowBias);

			// g_ChunkMeshRenderShader->SetUniform1i("uShaded", 1);

			glEnable(GL_CULL_FACE);

			g_World->Render(g_ChunkMeshRenderShader, MeshType::OPAQUE);

			glDisable(GL_CULL_FACE); // Disable FACE CULLING before rendering the translucent mesh?

			// g_ChunkMeshRenderShader->SetUniform1i("uShaded", 0);

			g_World->Render(g_ChunkMeshRenderShader, MeshType::TRANSLUCENT);

			g_ChunkMeshRenderShader->Unbind();

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		}

		// Render the sun/moon.
		{
			glEnable(GL_DEPTH_TEST);

			g_CubeRenderShader->Bind();
			g_CubeRenderShader->SetUniformMatrix4fv("uViewMatrix", g_Player->GetViewMatrix());
			g_CubeRenderShader->SetUniform3f("uColor", glm::vec3(1.0f, 1.0f, 0.25f));

			g_Sun->Render(g_CubeRenderShader);

			g_CubeRenderShader->Unbind();

			glDisable(GL_DEPTH_TEST);
		}

		// Render the UI.
		{
			glEnable(GL_BLEND);

			const glm::vec3& playerPosition = g_Player->GetPosition();
			const glm::vec3& playerDirection = g_Player->GetViewDirection();

			g_HUD->Render(2);

			std::string debugText1 = "PLAYER POSITION: (" + std::to_string(playerPosition.x) + ", " + std::to_string(playerPosition.y) + ", " + std::to_string(playerPosition.z) + ")";
			std::string debugText2 = "CAMERA DIRECTION: (" + std::to_string(playerDirection.x) + ", " + std::to_string(playerDirection.y) + ", " + std::to_string(playerDirection.z) + ")";
			std::string debugText3 = "GAME MODE: " + std::string(m_GameMode == GameMode::DEBUG ? "DEBUG" : "SURVIVAL");
			std::string debugText4 = "SELECTED BLOCK: " + g_Inventory->GetBlockNameOnSelectedSlot();
	
			g_TextRenderer->Write(debugText1, 16.0f, 16.0f, 1.0f, glm::vec3(1.0f), 3);
			g_TextRenderer->Write(debugText2, 16.0f, 32.0f, 1.0f, glm::vec3(1.0f), 3);
			g_TextRenderer->Write(debugText3, 16.0f, 48.0f, 1.0f, glm::vec3(1.0f), 3);
			g_TextRenderer->Write(debugText4, 16.0f, 64.0f, 1.0f, glm::vec3(1.0f), 3);

			std::string infoText1 = "PRESS 'F1/F2' TO CHANGE THE GAMEMODE";
			std::string infoText2 = "PRESS '<' '>' TO NAVIGATE THROUGH THE INVENTORY";
			std::string infoText3 = "PRESS 'F5' TO SHOW/HIDE THE DEPTH/SHADOW MAP";
			std::string infoText4 = "PRESS 'F6' TO SHOW/HIDE THE OCCLUSION MAP";
			std::string infoText5 = "PRESS 'ESC' TO QUIT";

			g_TextRenderer->Write(infoText1, 16.0f, m_ScreenHeight - 96.0f, 1.0f, glm::vec3(1.0f), 3);
			g_TextRenderer->Write(infoText2, 16.0f, m_ScreenHeight - 80.0f, 1.0f, glm::vec3(1.0f), 3);
			g_TextRenderer->Write(infoText3, 16.0f, m_ScreenHeight - 64.0f, 1.0f, glm::vec3(1.0f), 3);
			g_TextRenderer->Write(infoText4, 16.0f, m_ScreenHeight - 48.0f, 1.0f, glm::vec3(1.0f), 3);
			g_TextRenderer->Write(infoText5, 16.0f, m_ScreenHeight - 32.0f, 1.0f, glm::vec3(1.0f), 3);

			glDisable(GL_BLEND);
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

#pragma once

#include <map>
#include <tuple>
#include <array>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "../graphics/core/Shader.h"

#include "../physics/Ray.h"
#include "../physics/AABB.h"

#include "Block.h"
#include "Chunk.h"

class World
{
public:
	World(int seed);
	~World();

	int GetSeed();

	void InsertChunk(const Chunk& chunk);
	void RemoveChunk(const std::pair<int, int>& position);

	bool InsertBlockAt(const std::pair<int, int>& chunkPosition, const Block& block, const glm::ivec3& blockPosition);
	bool RemoveBlockAt(const std::pair<int, int>& chunkPosition, const glm::ivec3& blockPosition);

	void Setup(const std::pair<int, int>& origin, int stride);
	void Update(const std::pair<int, int>& origin, int stride, float deltaTime);
	void Render(Shader* shaderProgram);

	void GenerateMeshes();
	void UpdateChunkMesh(const std::pair<int, int>& chunkPosition);

	Intersection CastRay(const glm::vec3& origin, const glm::vec3& direction, float length);
	bool CheckCollision(const AABB& aabb, float maxRange);

private:
	int m_Seed;

	std::map<std::pair<int, int>, Chunk> m_Chunks;
	std::vector<std::pair<int, int>> m_ChunksToBeLoaded;

	Chunk* GetChunkIfExists(const std::pair<int, int>& position);
};

#pragma once

#include <map>
#include <tuple>

#include <glm/glm.hpp>

#include "../graphics/core/Shader.h"

#include "../util/Ray.h"

#include "Block.h"
#include "Chunk.h"

class World
{
public:
	World();
	~World();

	void InsertChunk(const Chunk& chunk);
	void RemoveChunkAt(const std::pair<int, int>& position);

	void InsertChunkBlockAt(const Chunk& chunk, const Block& block, const glm::ivec3& position);
	void RemoveChunkBlockAt(const Chunk& chunk, const glm::ivec3& position);

	Chunk& GetChunkAt(const std::pair<int, int>& position);

	void GenerateMeshes();

	void Render(Shader* shaderProgram);

	void CastRay(const glm::vec3& origin, const glm::vec3& direction, float length);

private:
	std::map<std::pair<int, int>, Chunk> m_Chunks;

	Chunk* GetChunkIfExists(const std::pair<int, int>& position);
};

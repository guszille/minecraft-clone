#pragma once

#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "../graphics/core/Shader.h"

#include "Chunk.h"
#include "Block.h"

class World
{
public:
	World();
	~World();

	void InsertChunk(const Chunk& chunk);
	void RemoveChunkAt(const std::pair<int, int>& position);

	void InsertChunkBlock(const Chunk& chunk, const Block& block);
	void RemoveChunkBlockAt(const Chunk& chunk, const glm::ivec3& position);

	Chunk& GetChunkAt(const std::pair<int, int>& position);

	void GenerateMeshes();

	void Render(Shader* shaderProgram);

private:
	std::map<std::pair<int, int>, Chunk> m_Chunks;

	Chunk* GetChunkIfExists(const std::pair<int, int>& position);
};

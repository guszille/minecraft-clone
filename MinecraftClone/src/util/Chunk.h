#pragma once

#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "../graphics/core/Shader.h"

#include "../graphics/Mesh.h"

#include "Block.h"

class Chunk
{
public:
	std::pair<int, int> m_Position;

	static glm::ivec3 s_DefaultDimensions;
	static int s_DefaultYPosition;

public:
	Chunk();
	Chunk(const std::pair<int, int>& position);
	~Chunk();

	void InsertBlock(const Block& block);
	void RemoveBlockAt(const glm::ivec3& position);

	Block& GetBlockAt(const glm::ivec3& position);

	void GenerateMesh(Chunk* chunksArround[4]);
	void UpdateMesh();

	void Render(Shader* shaderProgram);

	void Clear();

private:
	Block*** m_Blocks;
	Mesh m_Mesh;
};

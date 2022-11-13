#pragma once

#include <map>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../graphics/core/Shader.h"
#include "../graphics/Mesh.h"

#include "../util/Ray.h"

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

	void InsertBlockAt(const Block& block, const glm::ivec3& position);
	void RemoveBlockAt(const glm::ivec3& position);

	Block& GetBlockAt(const glm::ivec3& position);

	void GenerateMesh(Chunk* chunksArround[4]);
	void UpdateMesh();

	void Render(Shader* shaderProgram);

	Collision Intersect(const Ray& ray);

	void Clear();

private:
	Block*** m_Blocks;
	Mesh m_Mesh;

	static glm::ivec3 GetNextLocalBlockPosition(const glm::ivec3& position, const glm::ivec3& direction);
};

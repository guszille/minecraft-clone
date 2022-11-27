#pragma once

#include <map>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../graphics/core/Shader.h"
#include "../graphics/Mesh.h"

#include "../physics/Ray.h"
#include "../physics/AABB.h"

#include "Block.h"
#include "NoiseGenerator.h"

class Chunk
{
public:
	std::pair<int, int> m_Position;

	int** m_HeightMap;

	static glm::ivec3 s_DefaultDimensions;
	static int s_DefaultYPosition;

public:
	Chunk();
	Chunk(const std::pair<int, int>& position);
	~Chunk();

	bool InsertBlockAt(const Block& block, const glm::ivec3& position);
	bool RemoveBlockAt(const glm::ivec3& position);

	Block& GetBlockAt(const glm::ivec3& position);

	void GenerateHeightMap(int minYPosition = 0);
	void GenerateBlocks();

	void GenerateMesh(Chunk* chunksArround[4]);
	void UpdateMesh(Chunk* chunksArround[4]);

	void Render(Shader* shaderProgram);

	Intersection Intersect(const Ray& ray);
	bool CheckCollision(const AABB& aabb, float maxRange);

	void Clear();

	static bool IsAValidPosition(const glm::ivec3& position);
	static glm::ivec3 GetNextLocalBlockPosition(const glm::ivec3& position, const glm::ivec3& direction);
	static std::pair<int, int> GetChunkPositionFromWorld(const glm::vec3& position);

private:
	Block*** m_Blocks;

	Mesh m_Mesh;

	glm::vec3 m_DebugColor;

	void SampleRenderableFaces(Chunk* chunksArround[4]);
};

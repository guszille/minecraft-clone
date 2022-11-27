#pragma once

#include <array>

#include <glm/glm.hpp>

#include "../physics/Ray.h"
#include "../physics/AABB.h"

class Block
{
public:
	static enum Type { EMPTY = -1, GRASS, STONE, DIRTY, GRASS_SIDE, GLASS = 49 };
	static enum Face { FRONT, BACK, RIGHT, LEFT, TOP, BOTTOM };

	glm::vec3 m_Position; // Global poistion.

	static glm::vec3 s_CubeVertices[6][12];
	static glm::ivec3 s_CubeNormals[6];
	static glm::vec3 s_CubeColors[6];

	static unsigned int s_CubeIndices[6];
	static unsigned int s_CubeTexCoords[6][2];

public:
	Block();
	Block(Type type, const glm::vec3& position);
	~Block();

	Block::Type GetType();
	const std::array<glm::vec2, 4>& GetTexCoords();

	std::pair<bool, int> Intersect(const Ray& ray);
	bool CheckCollision(const AABB& aabb);

	static const std::array<glm::vec2, 4>& GenerateTexCoords(Block::Type type);

private:
	Block::Type m_Type;

	std::array<glm::vec2, 4> m_TexCoords;
};

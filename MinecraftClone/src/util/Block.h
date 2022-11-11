#pragma once

#include <array>

#include <glm/glm.hpp>

class Block
{
public:
	static enum Type { EMPTY = -1, GRASS, STONE, DIRTY, GRASS_SIDE, GLASS = 49 };
	static enum Face { FRONT, BACK, RIGHT, LEFT, TOP, BOTTOM };

	glm::ivec3 m_Position;

	static glm::vec3 s_CubeVertices[6][12];
	static glm::ivec3 s_CubeNormals[6];

	static unsigned int s_CubeIndices[6];
	static unsigned int s_CubeTexCoords[6][2];

public:
	Block();
	Block(Type type, const glm::ivec3& position);
	~Block();

	Block::Type GetType();
	const std::array<glm::vec2, 4>& GetTexCoords();

private:
	Block::Type m_Type;

	std::array<glm::vec2, 4> m_TexCoords;

	void GenerateTexCoords();
};

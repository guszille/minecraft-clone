#pragma once

#include <array>

#include <glm/glm.hpp>

#include "../physics/Ray.h"
#include "../physics/AABB.h"

enum BlockType {
	EMPTY = -1, // AIR.

	GRASS,
	SIDE_GRASS,
	DIRTY,
	STONE,
	BEDROCK,
	BRICKS,
	SAND,
	GLOWSTONE,
	SNOW,
	SIDE_SNOW,

	BIRCHWOOD,
	SIDE_BIRCHWOOD,
	OAKWOOD,
	SIDE_OAKWOOD,
	DARKOAKWOOD,
	SIDE_DARKOAKWOOD,
	ACACIAWOOD,
	SIDE_ACACIAWOOD,
	LAVA,
	WATER,

	BIRCHWOODPLANK = 30,
	OAKWOOKPLANK = 31,
	DARKOAKWOOKPLANK = 32,
	ACACIAWOODPLANK = 33,
	OAKLEAVES = 34,

	GRASSLEAVES = 40,
	ORANGETULIP = 41,
	PINKTULIP = 42,
	REDTULIP = 43,
	POPPY = 44,
	SUNFLOWER = 45,
	BLUEORCHID = 46,
	AZUREBLUET = 47,

	GLASS = 70
};

enum BlockFace { FRONT, BACK, RIGHT, LEFT, TOP, BOTTOM };

struct Cube
{
public:
	static glm::fvec3 s_Vertices[6][12];
	static glm::ivec3 s_Normals[6];
	static glm::fvec3 s_Colors[6];

	static unsigned int s_Indices[6];
	static unsigned int s_TexCoords[6][2];

	static int GetMostAlignedFace(const glm::vec3& direction);

private:
	Cube() = delete;  // Disable instantiating this class.
	~Cube() = delete;
};

class Block
{
public:
	glm::vec3 m_Position; // Global poistion.

	bool m_Solid, m_Translucent, m_SpecialShape;

public:
	Block(); Block(BlockType type, const glm::vec3& position, bool solid = true, bool translucent = false, bool specialShape = false);
	~Block();

	BlockType GetType();

	std::pair<bool, int> Intersect(const Ray& ray);
	bool CheckCollision(const AABB& aabb);

	static std::array<glm::vec2, 4> GenerateTexCoords(BlockType type);

private:
	BlockType m_Type;
};

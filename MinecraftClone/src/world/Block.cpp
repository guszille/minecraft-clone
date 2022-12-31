#include "Block.h"

glm::fvec3 Cube::s_Vertices[6][12] = {
	{ // front
		glm::fvec3(-0.5f, -0.5f,  0.5f),
		glm::fvec3( 0.5f, -0.5f,  0.5f),
		glm::fvec3( 0.5f,  0.5f,  0.5f),
		glm::fvec3(-0.5f,  0.5f,  0.5f)
	},
	{ // back
		glm::fvec3( 0.5f, -0.5f, -0.5f),
		glm::fvec3(-0.5f, -0.5f, -0.5f),
		glm::fvec3(-0.5f,  0.5f, -0.5f),
		glm::fvec3( 0.5f,  0.5f, -0.5f)
	},
	{ // right
		glm::fvec3( 0.5f, -0.5f,  0.5f),
		glm::fvec3( 0.5f, -0.5f, -0.5f),
		glm::fvec3( 0.5f,  0.5f, -0.5f),
		glm::fvec3( 0.5f,  0.5f,  0.5f)
	},
	{ // left
		glm::fvec3(-0.5f, -0.5f, -0.5f),
		glm::fvec3(-0.5f, -0.5f,  0.5f),
		glm::fvec3(-0.5f,  0.5f,  0.5f),
		glm::fvec3(-0.5f,  0.5f, -0.5f)
	},
	{ // top
		glm::fvec3(-0.5f,  0.5f,  0.5f),
		glm::fvec3( 0.5f,  0.5f,  0.5f),
		glm::fvec3( 0.5f,  0.5f, -0.5f),
		glm::fvec3(-0.5f,  0.5f, -0.5f)
	},
	{ // bottom
		glm::fvec3( 0.5f, -0.5f,  0.5f),
		glm::fvec3(-0.5f, -0.5f,  0.5f),
		glm::fvec3(-0.5f, -0.5f, -0.5f),
		glm::fvec3( 0.5f, -0.5f, -0.5f)
	}
};

glm::ivec3 Cube::s_Normals[6] = {
	glm::ivec3( 0, 0, 1), // front
	glm::ivec3( 0, 0,-1), // back
	glm::ivec3( 1, 0, 0), // right
	glm::ivec3(-1, 0, 0), // left
	glm::ivec3( 0, 1, 0), // top
	glm::ivec3( 0,-1, 0)  // bottom
};

// WARNING: Deprecated! Face colors to simulate a global directional light.
//
glm::fvec3 Cube::s_Colors[6] = {
	glm::fvec3(0.80f, 0.80f, 0.80f), // front
	glm::fvec3(0.80f, 0.80f, 0.80f), // back
	glm::fvec3(0.85f, 0.85f, 0.85f), // right
	glm::fvec3(0.85f, 0.85f, 0.85f), // left
	glm::fvec3(1.00f, 1.00f, 1.00f), // top
	glm::fvec3(1.00f, 1.00f, 1.00f)  // bottom
};

unsigned int Cube::s_Indices[6] = {
	0, 1, 2, 2, 3, 0
};

unsigned int Cube::s_TexCoords[6][2] = {
	{0, 0}, {1, 0}, {1, 1}, {0, 1}
};

int Cube::GetMostAlignedFace(const glm::vec3& direction)
{
	float greatestDot = 0.0f;
	int bestMatch = -1;

	for (int i = 0; i < 6; i++)
	{
		float currentDot = glm::dot(glm::normalize(direction), (glm::vec3)s_Normals[i]);

		if (currentDot >= greatestDot)
		{
			greatestDot = currentDot;
			bestMatch = i;
		}
	}

	return bestMatch;
}

Block::Block()
	: m_Type(BlockType::EMPTY), m_Position(), m_Solid(false), m_Translucent(false), m_SpecialShape(false)
{
}

Block::Block(BlockType type, const glm::vec3& position, bool solid, bool translucent, bool specialShape)
	: m_Type(type), m_Position(position), m_Solid(solid), m_Translucent(translucent), m_SpecialShape(specialShape)
{
}

Block::~Block()
{
}

BlockType Block::GetType()
{
	return m_Type;
}

std::pair<bool, int> Block::Intersect(const Ray& ray)
{
	glm::vec3 bounds[2] = { m_Position - glm::vec3(0.5f), m_Position + glm::vec3(0.5f) };
	int signs[3] = { (ray.m_InverseDirection.x < 0.0f), (ray.m_InverseDirection.y < 0.0f), (ray.m_InverseDirection.z < 0.0f) };

	float xMin = (bounds[signs[0]].x - ray.m_Origin.x) * ray.m_InverseDirection.x;
	float xMax = (bounds[1 - signs[0]].x - ray.m_Origin.x) * ray.m_InverseDirection.x;
	float yMin = (bounds[signs[1]].y - ray.m_Origin.y) * ray.m_InverseDirection.y;
	float yMax = (bounds[1 - signs[1]].y - ray.m_Origin.y) * ray.m_InverseDirection.y;
	float zMin = (bounds[signs[2]].z - ray.m_Origin.z) * ray.m_InverseDirection.z;
	float zMax = (bounds[1 - signs[2]].z - ray.m_Origin.z) * ray.m_InverseDirection.z;

	float tMin = xMin;
	float tMax = xMax;

	if (tMin > yMax || yMin > tMax)
	{
		return std::make_pair(false, -1);
	}

	if (yMin > tMin) { tMin = yMin; }
	if (yMax < tMax) { tMax = yMax; }

	if (tMin > zMax || zMin > tMax)
	{
		return std::make_pair(false, -1);
	}

	if (zMin > tMin) { tMin = zMin; }
	if (zMax < tMax) { tMax = zMax; }

	if (tMin < 0.0f || tMax < 0.0f)
	{
		return std::make_pair(false, -1);
	}

	std::pair<bool, int> intersection(true, -1);
	glm::vec3 P = ray.m_Origin + ray.m_Direction * tMin;

	for (unsigned int i = 0; i < 6; i++)
	{
		glm::vec3 N = (glm::vec3)Cube::s_Normals[i];
		glm::vec3 V = P - (i % 2 == 0 ? bounds[1] : bounds[0]); // Even index could be FRONT, RIGHT or TOP. Odd index could be BACK, LEFT or BOTTOM.

		float dot = glm::dot(N, V);

		if (dot == 0.0f)
		{
			intersection.second = i;
			break;
		}
	}

	return intersection;
}

bool Block::CheckCollision(const AABB& aabb)
{
	glm::vec3 bounds[2] = { m_Position - glm::vec3(0.5f), m_Position + glm::vec3(0.5f) };
	bool axisIntersections[3] = { true, true, true };

	axisIntersections[0] = aabb.m_Min.x <= bounds[1].x && aabb.m_Max.x >= bounds[0].x;
	axisIntersections[1] = aabb.m_Min.y <= bounds[1].y && aabb.m_Max.y >= bounds[0].y;
	axisIntersections[2] = aabb.m_Min.z <= bounds[1].z && aabb.m_Max.z >= bounds[0].z;

	return axisIntersections[0] && axisIntersections[1] && axisIntersections[2];
}

std::array<glm::vec2, 4> Block::GenerateTexCoords(BlockType type)
{
	std::array<glm::vec2, 4> uv{};

	if (type != BlockType::EMPTY)
	{
		// WARNING:
		// 
		// The sprite sheet dimensions are hard-coded setted here.
		// We expect a width and a height of 160 pixels, both. Also, each sprite with 16x16 pixels.
		//
		float size = 160.0f / 10.0f;
		float x0 = size * (type % 10);
		float x1 = x0 + size;
		float y0 = 160.0f - (size * (1 + (int)(type / 10.0f)));
		float y1 = y0 + size;

		x0 /= 160.0f;
		x1 /= 160.0f;
		y0 /= 160.0f;
		y1 /= 160.0f;

		uv[0] = glm::vec2(x0, y0);
		uv[1] = glm::vec2(x1, y0);
		uv[2] = glm::vec2(x1, y1);
		uv[3] = glm::vec2(x0, y1);
	}

	return uv;
}

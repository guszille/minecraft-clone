#include "Block.h"

glm::vec3 Block::s_CubeVertices[6][12] = {
	{ // front
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f)
	},
	{ // back
		glm::vec3( 0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3( 0.5f,  0.5f, -0.5f)
	},
	{ // right
		glm::vec3( 0.5f, -0.5f,  0.5f),
		glm::vec3( 0.5f, -0.5f, -0.5f),
		glm::vec3( 0.5f,  0.5f, -0.5f),
		glm::vec3( 0.5f,  0.5f,  0.5f)
	},
	{ // left
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f)
	},
	{ // top
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f,  0.5f),
		glm::vec3( 0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f)
	},
	{ // bottom
		glm::vec3( 0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3( 0.5f, -0.5f, -0.5f)
	}
};

glm::ivec3 Block::s_CubeNormals[6] = {
	glm::ivec3( 0, 0, 1), // front
	glm::ivec3( 0, 0,-1), // back
	glm::ivec3( 1, 0, 0), // right
	glm::ivec3(-1, 0, 0), // left
	glm::ivec3( 0, 1, 0), // top
	glm::ivec3( 0,-1, 0)  // bottom
};

glm::vec3 Block::s_CubeColors[6] = {
	glm::vec3(0.80f, 0.80f, 0.80f), // front
	glm::vec3(0.80f, 0.80f, 0.80f), // back
	glm::vec3(0.85f, 0.85f, 0.85f), // right
	glm::vec3(0.85f, 0.85f, 0.85f), // left
	glm::vec3(1.00f, 1.00f, 1.00f), // top
	glm::vec3(1.00f, 1.00f, 1.00f)  // bottom
};

unsigned int Block::s_CubeIndices[6] = {
	0, 1, 2, 2, 3, 0
};

unsigned int Block::s_CubeTexCoords[6][2] = {
	{0, 0}, {1, 0}, {1, 1}, {0, 1}
};

Block::Block()
	: m_Type(Block::Type::EMPTY), m_TexCoords(), m_Position()
{
}

Block::Block(Type type, const glm::vec3& position)
	: m_Type(type), m_TexCoords(GenerateTexCoords(type)), m_Position(position)
{
}

Block::~Block()
{
}

Block::Type Block::GetType()
{
	return m_Type;
}

const std::array<glm::vec2, 4>& Block::GetTexCoords()
{
	return m_TexCoords;
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

	if (yMin > tMin) tMin = yMin;
	if (yMax < tMax) tMax = yMax;

	if (tMin > zMax || zMin > tMax)
	{
		return std::make_pair(false, -1);
	}

	if (zMin > tMin) tMin = zMin;
	if (zMax < tMax) tMax = zMax;

	if (tMin < 0.0f || tMax < 0.0f)
	{
		return std::make_pair(false, -1);
	}

	std::pair<bool, int> intersection(true, -1);
	glm::vec3 P = ray.m_Origin + ray.m_Direction * tMin;

	for (unsigned int i = 0; i < 6; i++)
	{
		glm::vec3 N = (glm::vec3)s_CubeNormals[i];
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

const std::array<glm::vec2, 4>& Block::GenerateTexCoords(Block::Type type)
{
	std::array<glm::vec2, 4> uv;

	if (type != Block::Type::EMPTY)
	{
		// WARNING:
		// 
		// The sprite sheet dimensions are hard-coded setted here.
		// We expect a width and a height of 512 pixels, both. Also, each sprite with 16x16 pixels.
		//
		float size = 512.0f / 16.0f;
		float x0 = size * (type % 16);
		float x1 = x0 + size;
		float y0 = 512.0f - (size * (1 + (int)(type / 16.0f)));
		float y1 = y0 + size;

		x0 /= 512.0f;
		x1 /= 512.0f;
		y0 /= 512.0f;
		y1 /= 512.0f;

		uv[0] = glm::vec2(x0, y0);
		uv[1] = glm::vec2(x1, y0);
		uv[2] = glm::vec2(x1, y1);
		uv[3] = glm::vec2(x0, y1);
	}

	return uv;
}

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

Block::Block(Type type, const glm::ivec3& position)
	: m_Type(type), m_TexCoords(), m_Position(position)
{
	GenerateTexCoords();
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

void Block::GenerateTexCoords()
{
	if (m_Type != Block::Type::EMPTY)
	{
		// WARNING:
		// 
		// The sprite sheet dimensions are hard-coded setted here.
		// We expect a width and a height of 512 pixels, both. Also, each sprite with 16x16 pixels.
		//
		float size = 512.0f / 16.0f;
		float x0 = size * (m_Type % 16);
		float x1 = x0 + size;
		float y0 = 512.0f - (size * (1 + (int)(m_Type / 16.0f)));
		float y1 = y0 + size;

		x0 /= 512.0f;
		x1 /= 512.0f;
		y0 /= 512.0f;
		y1 /= 512.0f;

		m_TexCoords[0] = glm::vec2(x0, y0);
		m_TexCoords[1] = glm::vec2(x1, y0);
		m_TexCoords[2] = glm::vec2(x1, y1);
		m_TexCoords[3] = glm::vec2(x0, y1);
	}
}

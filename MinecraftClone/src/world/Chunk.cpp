#include "Chunk.h"

glm::ivec3 Chunk::s_DefaultDimensions(16, 256, 16);
int Chunk::s_DefaultYPosition = -128;

Chunk::Chunk()
	: m_Position(), m_Blocks(nullptr), m_DebugColor()
{
}

Chunk::Chunk(const std::pair<int, int>& position)
	: m_Position(position), m_DebugColor((float)std::rand() / RAND_MAX, (float)std::rand() / RAND_MAX, (float)std::rand() / RAND_MAX)
{
	m_Blocks = new Block**[s_DefaultDimensions.x];

	for (unsigned int i = 0; i < s_DefaultDimensions.x; i++)
	{
		m_Blocks[i] = new Block*[s_DefaultDimensions.y];

		for (unsigned int j = 0; j < s_DefaultDimensions.y; j++)
		{
			m_Blocks[i][j] = new Block[s_DefaultDimensions.z];
		}
	}
}

Chunk::~Chunk()
{
}

void Chunk::InsertBlockAt(const Block& block, const glm::ivec3& position)
{
	if (IsAValidPosition(position))
	{
		m_Blocks[position.x][position.y][position.z] = block;
	}
}

void Chunk::RemoveBlockAt(const glm::ivec3& position)
{
	if (IsAValidPosition(position))
	{
		m_Blocks[position.x][position.y][position.z] = Block();
	}
}

Block& Chunk::GetBlockAt(const glm::ivec3& position)
{
	if (IsAValidPosition(position))
	{
		return m_Blocks[position.x][position.y][position.z];
	}

	throw("The provided position is out of chunk!");
}

void Chunk::GenerateMesh(Chunk* chunksArround[4])
{
	SampleRenderableFaces(chunksArround);

	m_Mesh.GenerateRenderData();
}

void Chunk::UpdateMesh(Chunk* chunksArround[4])
{
	m_Mesh.m_Vertices.clear();
	m_Mesh.m_Indices.clear();

	m_Mesh.m_NumberOfFaces = 0;

	SampleRenderableFaces(chunksArround);
	
	m_Mesh.UpdateRenderData();
}

void Chunk::Render(Shader* shaderProgram)
{
	glm::vec3 chunkPosition(m_Position.first * s_DefaultDimensions.x, s_DefaultYPosition, m_Position.second * s_DefaultDimensions.z);

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, chunkPosition);

	shaderProgram->SetUniformMatrix4fv("uModelMatrix", modelMatrix);

	m_Mesh.Render();
}

Intersection Chunk::Intersect(const Ray& ray)
{
	Intersection intersection = std::make_tuple(false, std::make_pair(0, 0), glm::ivec3(0), glm::vec3(0.0f), -1);
	float distanceToClosestBlock = 0.0f;

	for (unsigned int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (unsigned int y = 0; y < s_DefaultDimensions.y; y++)
		{
			for (unsigned int z = 0; z < s_DefaultDimensions.z; z++)
			{
				Block& block = m_Blocks[x][y][z];

				if (block.GetType() != Block::Type::EMPTY)
				{
					float distanceToCurrBlock = glm::length(block.m_Position - ray.m_Origin);

					if (distanceToCurrBlock <= ray.m_Length)
					{
						std::pair<bool, int> blockIntersection = block.Intersect(ray);

						if (blockIntersection.first)
						{
							if (!std::get<0>(intersection) || distanceToCurrBlock < distanceToClosestBlock)
							{
								intersection = std::make_tuple(true, m_Position, glm::ivec3(x, y, z), block.m_Position, blockIntersection.second);

								distanceToClosestBlock = distanceToCurrBlock;
							}
						}
					}
				}
			}
		}
	}

	return intersection;
}

void Chunk::Clear()
{
	if (m_Blocks)
	{
		for (unsigned int i = 0; i < s_DefaultDimensions.x; i++)
		{
			for (unsigned int j = 0; j < s_DefaultDimensions.y; j++)
			{
				delete[] m_Blocks[i][j];
			}

			delete[] m_Blocks[i];
		}

		delete[] m_Blocks;
	}

	m_Mesh.ClearRenderData();
}

void Chunk::SampleRenderableFaces(Chunk* chunksArround[4])
{
	for (unsigned int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (unsigned int y = 0; y < s_DefaultDimensions.y; y++)
		{
			for (unsigned int z = 0; z < s_DefaultDimensions.z; z++)
			{
				Block& block = m_Blocks[x][y][z];
				glm::ivec3 localBlockPosition(x, y, z);

				if (block.GetType() != Block::Type::EMPTY)
				{
					for (unsigned int i = 0; i < 6; i++) // Iterate over all block faces.
					{
						bool faceCanComposeMesh = false;

						if (i == Block::Face::FRONT)
						{
							if (localBlockPosition.z == s_DefaultDimensions.z - 1)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Block::s_CubeNormals[i]);
									Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

									if (obstructingBlock.GetType() == Block::Type::EMPTY)
									{
										faceCanComposeMesh = true;
									}
								}
								else
								{
									faceCanComposeMesh = true;
								}
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::BACK)
						{
							if (localBlockPosition.z == 0)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Block::s_CubeNormals[i]);
									Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

									if (obstructingBlock.GetType() == Block::Type::EMPTY)
									{
										faceCanComposeMesh = true;
									}
								}
								else
								{
									faceCanComposeMesh = true;
								}
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::RIGHT)
						{
							if (localBlockPosition.x == s_DefaultDimensions.x - 1)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Block::s_CubeNormals[i]);
									Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

									if (obstructingBlock.GetType() == Block::Type::EMPTY)
									{
										faceCanComposeMesh = true;
									}
								}
								else
								{
									faceCanComposeMesh = true;
								}
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::LEFT)
						{
							if (localBlockPosition.x == 0)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Block::s_CubeNormals[i]);
									Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

									if (obstructingBlock.GetType() == Block::Type::EMPTY)
									{
										faceCanComposeMesh = true;
									}
								}
								else
								{
									faceCanComposeMesh = true;
								}
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::TOP)
						{
							if (localBlockPosition.y == s_DefaultDimensions.y - 1)
							{
								faceCanComposeMesh = true;
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::BOTTOM)
						{
							if (localBlockPosition.y == 0)
							{
								faceCanComposeMesh = true;
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}

						if (faceCanComposeMesh)
						{
							glm::vec3* vertices = Block::s_CubeVertices[i];
							unsigned int* indices = Block::s_CubeIndices; // Same indices to all block faces.

							std::array<glm::vec2, 4> uv = block.GetTexCoords();
							glm::vec3 c = Block::s_CubeColors[i];

							// FIXME: Using only for debug.
							// c = c * m_DebugColor;

							if (block.GetType() == Block::Type::GRASS)
							{
								if (i == Block::Face::TOP)
								{
									c = c * glm::vec3(0.0f, 1.0f, 0.0f);
								}
								else
								{
									if (i == Block::Face::BOTTOM)
									{
										uv = Block::GenerateTexCoords(Block::Type::DIRTY);
									}
									else
									{
										uv = Block::GenerateTexCoords(Block::Type::GRASS_SIDE);
									}
								}
							}

							glm::vec3 v0 = vertices[0] + (glm::vec3)localBlockPosition;
							glm::vec3 v1 = vertices[1] + (glm::vec3)localBlockPosition;
							glm::vec3 v2 = vertices[2] + (glm::vec3)localBlockPosition;
							glm::vec3 v3 = vertices[3] + (glm::vec3)localBlockPosition;

							m_Mesh.m_Vertices.push_back(v0.x);
							m_Mesh.m_Vertices.push_back(v0.y);
							m_Mesh.m_Vertices.push_back(v0.z);
							m_Mesh.m_Vertices.push_back(uv[0].x);
							m_Mesh.m_Vertices.push_back(uv[0].y);
							m_Mesh.m_Vertices.push_back(c.r);
							m_Mesh.m_Vertices.push_back(c.g);
							m_Mesh.m_Vertices.push_back(c.b);

							m_Mesh.m_Vertices.push_back(v1.x);
							m_Mesh.m_Vertices.push_back(v1.y);
							m_Mesh.m_Vertices.push_back(v1.z);
							m_Mesh.m_Vertices.push_back(uv[1].x);
							m_Mesh.m_Vertices.push_back(uv[1].y);
							m_Mesh.m_Vertices.push_back(c.r);
							m_Mesh.m_Vertices.push_back(c.g);
							m_Mesh.m_Vertices.push_back(c.b);

							m_Mesh.m_Vertices.push_back(v2.x);
							m_Mesh.m_Vertices.push_back(v2.y);
							m_Mesh.m_Vertices.push_back(v2.z);
							m_Mesh.m_Vertices.push_back(uv[2].x);
							m_Mesh.m_Vertices.push_back(uv[2].y);
							m_Mesh.m_Vertices.push_back(c.r);
							m_Mesh.m_Vertices.push_back(c.g);
							m_Mesh.m_Vertices.push_back(c.b);

							m_Mesh.m_Vertices.push_back(v3.x);
							m_Mesh.m_Vertices.push_back(v3.y);
							m_Mesh.m_Vertices.push_back(v3.z);
							m_Mesh.m_Vertices.push_back(uv[3].x);
							m_Mesh.m_Vertices.push_back(uv[3].y);
							m_Mesh.m_Vertices.push_back(c.r);
							m_Mesh.m_Vertices.push_back(c.g);
							m_Mesh.m_Vertices.push_back(c.b);

							m_Mesh.m_Indices.push_back(indices[0] + (m_Mesh.m_NumberOfFaces * 4));
							m_Mesh.m_Indices.push_back(indices[1] + (m_Mesh.m_NumberOfFaces * 4));
							m_Mesh.m_Indices.push_back(indices[2] + (m_Mesh.m_NumberOfFaces * 4));
							m_Mesh.m_Indices.push_back(indices[3] + (m_Mesh.m_NumberOfFaces * 4));
							m_Mesh.m_Indices.push_back(indices[4] + (m_Mesh.m_NumberOfFaces * 4));
							m_Mesh.m_Indices.push_back(indices[5] + (m_Mesh.m_NumberOfFaces * 4));

							m_Mesh.m_NumberOfFaces += 1;
						}
					}
				}
			}
		}
	}
}

bool Chunk::IsAValidPosition(const glm::ivec3& position)
{
	if (position.x >= 0 && position.x < s_DefaultDimensions.x)
	{
		if (position.y >= 0 && position.y < s_DefaultDimensions.y)
		{
			if (position.z >= 0 && position.z < s_DefaultDimensions.z)
			{
				return true;
			}
		}
	}

	return false;
}

glm::ivec3 Chunk::GetNextLocalBlockPosition(const glm::ivec3& position, const glm::ivec3& direction)
{
	glm::ivec3 nextPosition = position + direction;

	if (nextPosition.x < 0) { nextPosition.x = 15; }

	if (nextPosition.y < 0) { nextPosition.y = 15; }

	if (nextPosition.z < 0) { nextPosition.z = 15; }

	nextPosition.x = nextPosition.x % s_DefaultDimensions.x;
	nextPosition.y = nextPosition.y % s_DefaultDimensions.y;
	nextPosition.z = nextPosition.z % s_DefaultDimensions.z;

	return nextPosition;
}

std::pair<int, int> Chunk::GetChunkPositionFromWorld(const glm::vec3& position)
{
	int x = position.x > 0.0f ? (position.x / s_DefaultDimensions.x) : ((position.x - (s_DefaultDimensions.x - 1)) / s_DefaultDimensions.x);
	int z = position.z > 0.0f ? (position.z / s_DefaultDimensions.z) : ((position.z - (s_DefaultDimensions.z - 1)) / s_DefaultDimensions.z);

	return std::pair<int, int>(x, z);
}

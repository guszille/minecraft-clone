#include "Chunk.h"

glm::ivec3 Chunk::s_DefaultDimensions(16, 256, 16);
int Chunk::s_DefaultYPosition = -128;

Chunk::Chunk()
	: m_Position(), m_Blocks(nullptr)
{
}

Chunk::Chunk(const std::pair<int, int>& position)
	: m_Position(position)
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

void Chunk::InsertBlock(const Block& block)
{
	if (block.m_Position.x >= 0 && block.m_Position.x < s_DefaultDimensions.x)
	{
		if (block.m_Position.y >= 0 && block.m_Position.y < s_DefaultDimensions.y)
		{
			if (block.m_Position.z >= 0 && block.m_Position.z < s_DefaultDimensions.z)
			{
				m_Blocks[block.m_Position.x][block.m_Position.y][block.m_Position.z] = block;
			}
		}
	}
}

void Chunk::RemoveBlockAt(const glm::ivec3& position)
{
	if (position.x >= 0 && position.x < s_DefaultDimensions.x)
	{
		if (position.y >= 0 && position.y < s_DefaultDimensions.y)
		{
			if (position.z >= 0 && position.z < s_DefaultDimensions.z)
			{
				m_Blocks[position.x][position.y][position.z] = Block();
			}
		}
	}
}

Block& Chunk::GetBlockAt(const glm::ivec3& position)
{
	if (position.x >= 0 && position.x < s_DefaultDimensions.x)
	{
		if (position.y >= 0 && position.y < s_DefaultDimensions.y)
		{
			if (position.z >= 0 && position.z < s_DefaultDimensions.z)
			{
				return m_Blocks[position.x][position.y][position.z];
			}
		}
	}

	throw("The provided position is out of chunk!");
}

void Chunk::GenerateMesh(Chunk* chunksArround[4])
{
	for (unsigned int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (unsigned int y = 0; y < s_DefaultDimensions.y; y++)
		{
			for (unsigned int z = 0; z < s_DefaultDimensions.z; z++)
			{
				Block& block = m_Blocks[x][y][z];

				if (block.GetType() != Block::Type::EMPTY)
				{
					for (unsigned int i = 0; i < 6; i++) // Iterate over all block faces.
					{
						bool faceCanComposeMesh = false;

						if (i == Block::Face::FRONT)
						{
							if (block.m_Position.z == s_DefaultDimensions.z - 1)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									Block& obstructingBlock = chunk->GetBlockAt(glm::ivec3(block.m_Position.x, block.m_Position.y, 0));

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
								Block& obstructingBlock = GetBlockAt(block.m_Position + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::BACK)
						{
							if (block.m_Position.z == 0)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									Block& obstructingBlock = chunk->GetBlockAt(glm::ivec3(block.m_Position.x, block.m_Position.y, s_DefaultDimensions.z - 1));

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
								Block& obstructingBlock = GetBlockAt(block.m_Position + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::RIGHT)
						{
							if (block.m_Position.x == s_DefaultDimensions.x - 1)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									Block& obstructingBlock = chunk->GetBlockAt(glm::ivec3(0, block.m_Position.y, block.m_Position.z));

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
								Block& obstructingBlock = GetBlockAt(block.m_Position + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::LEFT)
						{
							if (block.m_Position.x == 0)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									Block& obstructingBlock = chunk->GetBlockAt(glm::ivec3(s_DefaultDimensions.x - 1, block.m_Position.y, block.m_Position.z));

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
								Block& obstructingBlock = GetBlockAt(block.m_Position + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::TOP)
						{
							if (block.m_Position.y == s_DefaultDimensions.y - 1)
							{
								faceCanComposeMesh = true;
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(block.m_Position + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == Block::Face::BOTTOM)
						{
							if (block.m_Position.y == 0)
							{
								faceCanComposeMesh = true;
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(block.m_Position + Block::s_CubeNormals[i]);

								if (obstructingBlock.GetType() == Block::Type::EMPTY)
								{
									faceCanComposeMesh = true;
								}
							}
						}

						if (faceCanComposeMesh)
						{
							glm::vec3* vertices = Block::s_CubeVertices[i];
							const std::array<glm::vec2, 4>& uv = block.GetTexCoords();
							unsigned int* indices = Block::s_CubeIndices;

							glm::mat4 blockModelMatrix = glm::mat4(1.0f);
							blockModelMatrix = glm::translate(blockModelMatrix, (glm::vec3)block.m_Position);

							glm::vec4 v0 = blockModelMatrix * glm::vec4(vertices[0], 1.0f);
							glm::vec4 v1 = blockModelMatrix * glm::vec4(vertices[1], 1.0f);
							glm::vec4 v2 = blockModelMatrix * glm::vec4(vertices[2], 1.0f);
							glm::vec4 v3 = blockModelMatrix * glm::vec4(vertices[3], 1.0f);

							m_Mesh.m_Vertices.push_back(v0.x);
							m_Mesh.m_Vertices.push_back(v0.y);
							m_Mesh.m_Vertices.push_back(v0.z);
							m_Mesh.m_Vertices.push_back(uv[0].x);
							m_Mesh.m_Vertices.push_back(uv[0].y);

							m_Mesh.m_Vertices.push_back(v1.x);
							m_Mesh.m_Vertices.push_back(v1.y);
							m_Mesh.m_Vertices.push_back(v1.z);
							m_Mesh.m_Vertices.push_back(uv[1].x);
							m_Mesh.m_Vertices.push_back(uv[1].y);

							m_Mesh.m_Vertices.push_back(v2.x);
							m_Mesh.m_Vertices.push_back(v2.y);
							m_Mesh.m_Vertices.push_back(v2.z);
							m_Mesh.m_Vertices.push_back(uv[2].x);
							m_Mesh.m_Vertices.push_back(uv[2].y);

							m_Mesh.m_Vertices.push_back(v3.x);
							m_Mesh.m_Vertices.push_back(v3.y);
							m_Mesh.m_Vertices.push_back(v3.z);
							m_Mesh.m_Vertices.push_back(uv[3].x);
							m_Mesh.m_Vertices.push_back(uv[3].y);

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

	m_Mesh.GenerateRenderData();
}

void Chunk::UpdateMesh()
{
	// TODO: Update mesh VBO.
}

void Chunk::Render(Shader* shaderProgram)
{
	glm::vec3 chunkPosition(m_Position.first * s_DefaultDimensions.x, s_DefaultYPosition, m_Position.second * s_DefaultDimensions.z);

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, chunkPosition);

	shaderProgram->SetUniformMatrix4fv("uModelMatrix", modelMatrix);

	m_Mesh.Render();
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
}

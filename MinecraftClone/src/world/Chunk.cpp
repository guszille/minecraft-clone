#include "Chunk.h"

glm::ivec3 Chunk::s_DefaultDimensions(16, 256, 16);
int Chunk::s_DefaultYPosition = -128;
int Chunk::s_DefaultLayerSize = 8;

Chunk::Chunk(const std::pair<int, int>& position)
	: m_Position(position)
{
	m_HeightMap = new int*[s_DefaultDimensions.x];

	for (int i = 0; i < s_DefaultDimensions.x; i++) { m_HeightMap[i] = new int[s_DefaultDimensions.z]; }

	m_Blocks = new Block**[s_DefaultDimensions.x];

	for (int i = 0; i < s_DefaultDimensions.x; i++) { m_Blocks[i] = new Block*[s_DefaultDimensions.y]; for (int j = 0; j < s_DefaultDimensions.y; j++) { m_Blocks[i][j] = new Block[s_DefaultDimensions.z]; } }
}

Chunk::~Chunk()
{
}

bool Chunk::InsertBlockAt(const Block& block, const glm::ivec3& position)
{
	if (IsAValidPosition(position))
	{
		if (m_Blocks[position.x][position.y][position.z].GetType() == BlockType::EMPTY)
		{
			m_Blocks[position.x][position.y][position.z] = block;

			return true;
		}
	}

	return false;
}

bool Chunk::RemoveBlockAt(const glm::ivec3& position)
{
	if (IsAValidPosition(position))
	{
		m_Blocks[position.x][position.y][position.z] = Block();

		return true;
	}

	return false;
}

Block& Chunk::GetBlockAt(const glm::ivec3& position)
{
	if (IsAValidPosition(position))
	{
		return m_Blocks[position.x][position.y][position.z];
	}

	throw("The provided position is out of chunk!");
}

void Chunk::GenerateHeightMap(int minYPosition)
{
	int chunkWorldPosition[3] = { m_Position.first * s_DefaultDimensions.x, s_DefaultYPosition, m_Position.second * s_DefaultDimensions.z };
	int maxYPosition = s_DefaultDimensions.y; // (int)((2.0f / 3.0f) * s_DefaultDimensions.y);

	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int z = 0; z < s_DefaultDimensions.z; z++)
		{
			float noise = NoiseGenerator::GetNoise2D((float)(x + chunkWorldPosition[0]), (float)(z + chunkWorldPosition[2]));

			m_HeightMap[x][z] = (int)(minYPosition + (((noise + 1.0f) / 2.0f) * maxYPosition));
		}
	}
}

void Chunk::GenerateBlocks()
{
	int chunkWorldPosition[3] = { m_Position.first * s_DefaultDimensions.x, s_DefaultYPosition, m_Position.second * s_DefaultDimensions.z };
	int maxYPosition = s_DefaultDimensions.y; // (int)((2.0f / 3.0f) * s_DefaultDimensions.y);
	int seaLevel = s_DefaultYPosition + s_DefaultDimensions.y;

	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int y = 0; y < maxYPosition; y++)
		{
			for (int z = 0; z < s_DefaultDimensions.z; z++)
			{
				int xWorldPos = x + chunkWorldPosition[0];
				int yWorldPos = y + chunkWorldPosition[1];
				int zWorldPos = z + chunkWorldPosition[2];
				int maxHeight = m_HeightMap[x][z];

				if (y <= maxHeight)
				{
					BlockType blockType = BlockType::EMPTY;

					if (y == maxHeight)
					{
						blockType = BlockType::GRASS;

						if (y < seaLevel - 1) { blockType = BlockType::SAND; } // TODO: Improve terrain/biome generation.
					}
					else if (y < maxHeight && y >= maxHeight / 2)
					{
						blockType = BlockType::DIRTY;
					}
					else if (y < maxHeight / 2)
					{
						blockType = BlockType::STONE;
					}

					InsertBlockAt(Block(blockType, glm::vec3(xWorldPos, yWorldPos, zWorldPos)), glm::ivec3(x, y, z));
				}
				else if (y < seaLevel)
				{
					BlockType blockType = BlockType::WATER;

					InsertBlockAt(Block(blockType, glm::vec3(xWorldPos, yWorldPos, zWorldPos), false, true), glm::ivec3(x, y, z));
				}
			}
		}
	}
}

void Chunk::GenerateMesh(Chunk* chunksArround[4])
{
	SampleRenderableFaces(chunksArround);

	m_OpaqueMesh.GenerateRenderData();

	m_TranslucentMesh.GenerateRenderData();
}

void Chunk::UpdateMesh(Chunk* chunksArround[4])
{
	m_OpaqueMesh.m_Vertices.clear();
	m_OpaqueMesh.m_Indices.clear();
	m_OpaqueMesh.m_NumberOfFaces = 0;

	m_TranslucentMesh.m_Vertices.clear();
	m_TranslucentMesh.m_Indices.clear();
	m_TranslucentMesh.m_NumberOfFaces = 0;

	SampleRenderableFaces(chunksArround);

	m_OpaqueMesh.UpdateRenderData();

	m_TranslucentMesh.UpdateRenderData();
}

void Chunk::Render(Shader* shaderProgram, MeshType meshTypeToRender)
{
	glm::vec3 chunkPosition(s_DefaultDimensions.x * m_Position.first, s_DefaultYPosition, s_DefaultDimensions.z * m_Position.second);
	
	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, chunkPosition);

	shaderProgram->SetUniformMatrix4fv("uModelMatrix", modelMatrix);

	if (meshTypeToRender == MeshType::OPAQUE)
	{
		m_OpaqueMesh.Render();
	}
	else if (meshTypeToRender == MeshType::TRANSLUCENT)
	{
		m_TranslucentMesh.Render();
	}
}

Intersection Chunk::Intersect(const Ray& ray)
{
	Intersection chunkIntersection = std::make_tuple(false, std::make_pair(0, 0), glm::ivec3(0), glm::vec3(0.0f), -1);
	float distanceToClosestBlock = 0.0f;

	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int y = 0; y < s_DefaultDimensions.y; y++)
		{
			for (int z = 0; z < s_DefaultDimensions.z; z++)
			{
				Block& block = m_Blocks[x][y][z];

				if (block.GetType() != BlockType::EMPTY && block.m_Solid)
				{
					float distanceToCurrBlock = glm::length(block.m_Position - ray.m_Origin);

					if (distanceToCurrBlock <= ray.m_Length)
					{
						std::pair<bool, int> blockIntersection = block.Intersect(ray);

						if (blockIntersection.first)
						{
							if (!std::get<0>(chunkIntersection) || distanceToCurrBlock < distanceToClosestBlock)
							{
								chunkIntersection = std::make_tuple(true, m_Position, glm::ivec3(x, y, z), block.m_Position, blockIntersection.second);

								distanceToClosestBlock = distanceToCurrBlock;
							}
						}
					}
				}
			}
		}
	}

	return chunkIntersection;
}

std::vector<Collision> Chunk::CheckCollisions(const AABB& aabb, float maxRange)
{
	std::vector<Collision> chunkCollisions;
	int aabbChunkLayer = GetChunkLayerFromWorld(aabb.m_Origin);

	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int y = 0; y < s_DefaultDimensions.y; y++)
		{
			int currentChunkLayer = (int)(y / s_DefaultLayerSize);

			if (currentChunkLayer >= (aabbChunkLayer - 1) && currentChunkLayer <= (aabbChunkLayer + 1))
			{
				for (int z = 0; z < s_DefaultDimensions.z; z++)
				{
					Block& block = m_Blocks[x][y][z];

					if (block.GetType() != BlockType::EMPTY && block.m_Solid)
					{
						glm::vec3 directionToAABB = aabb.m_Origin - block.m_Position;

						if (glm::length(directionToAABB) <= maxRange)
						{
							if (block.CheckCollision(aabb))
							{
								int collidedFace = Cube::GetMostAlignedFace(directionToAABB);

								if (collidedFace == BlockFace::TOP || collidedFace == BlockFace::BOTTOM) // WARNING: Doing a double check when the collision occurred on the top or bottom faces.
								{
									if (collidedFace == BlockFace::TOP)
									{
										glm::vec3 simulatedAABBOrigin = aabb.m_Origin - glm::vec3(0.0f, aabb.m_KnotOffset, 0.0f);
										directionToAABB = simulatedAABBOrigin - block.m_Position;
									}
									else
									{
										glm::vec3 simulatedAABBOrigin = aabb.m_Origin + glm::vec3(0.0f, aabb.m_KnotOffset, 0.0f);
										directionToAABB = simulatedAABBOrigin - block.m_Position;
									}

									collidedFace = Cube::GetMostAlignedFace(directionToAABB);
								}

								glm::vec3 cartesianDistanceToAABB = glm::abs(aabb.m_Origin - block.m_Position);
								Collision blockCollision = std::make_tuple(true, collidedFace, cartesianDistanceToAABB);

								chunkCollisions.push_back(blockCollision);
							}
						}
					}
				}
			}
		}
	}

	return chunkCollisions;
}

void Chunk::Clear()
{
	if (m_HeightMap)
	{
		for (int i = 0; i < s_DefaultDimensions.x; i++) { delete m_HeightMap[i]; }

		delete m_HeightMap;
	}

	if (m_Blocks)
	{
		for (int i = 0; i < s_DefaultDimensions.x; i++) { for (int j = 0; j < s_DefaultDimensions.y; j++) { delete[] m_Blocks[i][j]; } delete[] m_Blocks[i]; }

		delete[] m_Blocks;
	}

	m_OpaqueMesh.ClearRenderData();

	m_TranslucentMesh.ClearRenderData();
}

void Chunk::SampleRenderableFaces(Chunk* chunksArround[4])
{
	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int y = 0; y < s_DefaultDimensions.y; y++)
		{
			for (int z = 0; z < s_DefaultDimensions.z; z++)
			{
				Block& block = m_Blocks[x][y][z];
				glm::ivec3 localBlockPosition(x, y, z);

				if (block.GetType() != BlockType::EMPTY)
				{
					for (unsigned int i = 0; i < 6; i++) // Iterate over all block faces.
					{
						bool faceCanComposeMesh = false;

						if (i == BlockFace::FRONT)
						{
							if (localBlockPosition.z == s_DefaultDimensions.z - 1)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Cube::s_Normals[i]);
									Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

									if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
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
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[i]);

								if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == BlockFace::BACK)
						{
							if (localBlockPosition.z == 0)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Cube::s_Normals[i]);
									Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

									if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
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
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[i]);

								if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == BlockFace::RIGHT)
						{
							if (localBlockPosition.x == s_DefaultDimensions.x - 1)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Cube::s_Normals[i]);
									Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

									if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
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
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[i]);

								if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == BlockFace::LEFT)
						{
							if (localBlockPosition.x == 0)
							{
								Chunk* chunk = chunksArround[i];

								if (chunk != nullptr)
								{
									glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Cube::s_Normals[i]);
									Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

									if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
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
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[i]);

								if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == BlockFace::TOP)
						{
							if (localBlockPosition.y == s_DefaultDimensions.y - 1)
							{
								faceCanComposeMesh = true;
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[i]);

								if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
								{
									faceCanComposeMesh = true;
								}
							}
						}
						else if (i == BlockFace::BOTTOM)
						{
							if (localBlockPosition.y == 0)
							{
								faceCanComposeMesh = true;
							}
							else
							{
								Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[i]);

								if (obstructingBlock.GetType() == BlockType::EMPTY || (block.m_Solid && obstructingBlock.m_Translucent))
								{
									faceCanComposeMesh = true;
								}
							}
						}

						if (faceCanComposeMesh)
						{
							glm::vec3* vertices = Cube::s_Vertices[i];
							unsigned int* indices = Cube::s_Indices; // Same indices to all block faces.

							glm::vec3 n = (glm::vec3)Cube::s_Normals[i];
							std::array<glm::vec2, 4> uv = Block::GenerateTexCoords(block.GetType());

							glm::vec3 v0 = vertices[0] + (glm::vec3)localBlockPosition;
							glm::vec3 v1 = vertices[1] + (glm::vec3)localBlockPosition;
							glm::vec3 v2 = vertices[2] + (glm::vec3)localBlockPosition;
							glm::vec3 v3 = vertices[3] + (glm::vec3)localBlockPosition;

							if (!block.m_Translucent) // Opaque mesh.
							{
								if (block.GetType() == BlockType::GRASS)
								{
									if (i != BlockFace::TOP)
									{
										if (i == BlockFace::BOTTOM) { uv = Block::GenerateTexCoords(BlockType::DIRTY); }
										else { uv = Block::GenerateTexCoords(BlockType::SIDE_GRASS); }
									}
								}

								m_OpaqueMesh.m_Vertices.push_back(v0.x);
								m_OpaqueMesh.m_Vertices.push_back(v0.y);
								m_OpaqueMesh.m_Vertices.push_back(v0.z);
								m_OpaqueMesh.m_Vertices.push_back(n.x);
								m_OpaqueMesh.m_Vertices.push_back(n.y);
								m_OpaqueMesh.m_Vertices.push_back(n.z);
								m_OpaqueMesh.m_Vertices.push_back(uv[0].x);
								m_OpaqueMesh.m_Vertices.push_back(uv[0].y);

								m_OpaqueMesh.m_Vertices.push_back(v1.x);
								m_OpaqueMesh.m_Vertices.push_back(v1.y);
								m_OpaqueMesh.m_Vertices.push_back(v1.z);
								m_OpaqueMesh.m_Vertices.push_back(n.x);
								m_OpaqueMesh.m_Vertices.push_back(n.y);
								m_OpaqueMesh.m_Vertices.push_back(n.z);
								m_OpaqueMesh.m_Vertices.push_back(uv[1].x);
								m_OpaqueMesh.m_Vertices.push_back(uv[1].y);

								m_OpaqueMesh.m_Vertices.push_back(v2.x);
								m_OpaqueMesh.m_Vertices.push_back(v2.y);
								m_OpaqueMesh.m_Vertices.push_back(v2.z);
								m_OpaqueMesh.m_Vertices.push_back(n.x);
								m_OpaqueMesh.m_Vertices.push_back(n.y);
								m_OpaqueMesh.m_Vertices.push_back(n.z);
								m_OpaqueMesh.m_Vertices.push_back(uv[2].x);
								m_OpaqueMesh.m_Vertices.push_back(uv[2].y);

								m_OpaqueMesh.m_Vertices.push_back(v3.x);
								m_OpaqueMesh.m_Vertices.push_back(v3.y);
								m_OpaqueMesh.m_Vertices.push_back(v3.z);
								m_OpaqueMesh.m_Vertices.push_back(n.x);
								m_OpaqueMesh.m_Vertices.push_back(n.y);
								m_OpaqueMesh.m_Vertices.push_back(n.z);
								m_OpaqueMesh.m_Vertices.push_back(uv[3].x);
								m_OpaqueMesh.m_Vertices.push_back(uv[3].y);

								m_OpaqueMesh.m_Indices.push_back(indices[0] + (m_OpaqueMesh.m_NumberOfFaces * 4));
								m_OpaqueMesh.m_Indices.push_back(indices[1] + (m_OpaqueMesh.m_NumberOfFaces * 4));
								m_OpaqueMesh.m_Indices.push_back(indices[2] + (m_OpaqueMesh.m_NumberOfFaces * 4));
								m_OpaqueMesh.m_Indices.push_back(indices[3] + (m_OpaqueMesh.m_NumberOfFaces * 4));
								m_OpaqueMesh.m_Indices.push_back(indices[4] + (m_OpaqueMesh.m_NumberOfFaces * 4));
								m_OpaqueMesh.m_Indices.push_back(indices[5] + (m_OpaqueMesh.m_NumberOfFaces * 4));

								m_OpaqueMesh.m_NumberOfFaces += 1;
							}
							else // Translucent mesh.
							{
								m_TranslucentMesh.m_Vertices.push_back(v0.x);
								m_TranslucentMesh.m_Vertices.push_back(v0.y);
								m_TranslucentMesh.m_Vertices.push_back(v0.z);
								m_TranslucentMesh.m_Vertices.push_back(n.x);
								m_TranslucentMesh.m_Vertices.push_back(n.y);
								m_TranslucentMesh.m_Vertices.push_back(n.z);
								m_TranslucentMesh.m_Vertices.push_back(uv[0].x);
								m_TranslucentMesh.m_Vertices.push_back(uv[0].y);

								m_TranslucentMesh.m_Vertices.push_back(v1.x);
								m_TranslucentMesh.m_Vertices.push_back(v1.y);
								m_TranslucentMesh.m_Vertices.push_back(v1.z);
								m_TranslucentMesh.m_Vertices.push_back(n.x);
								m_TranslucentMesh.m_Vertices.push_back(n.y);
								m_TranslucentMesh.m_Vertices.push_back(n.z);
								m_TranslucentMesh.m_Vertices.push_back(uv[1].x);
								m_TranslucentMesh.m_Vertices.push_back(uv[1].y);

								m_TranslucentMesh.m_Vertices.push_back(v2.x);
								m_TranslucentMesh.m_Vertices.push_back(v2.y);
								m_TranslucentMesh.m_Vertices.push_back(v2.z);
								m_TranslucentMesh.m_Vertices.push_back(n.x);
								m_TranslucentMesh.m_Vertices.push_back(n.y);
								m_TranslucentMesh.m_Vertices.push_back(n.z);
								m_TranslucentMesh.m_Vertices.push_back(uv[2].x);
								m_TranslucentMesh.m_Vertices.push_back(uv[2].y);

								m_TranslucentMesh.m_Vertices.push_back(v3.x);
								m_TranslucentMesh.m_Vertices.push_back(v3.y);
								m_TranslucentMesh.m_Vertices.push_back(v3.z);
								m_TranslucentMesh.m_Vertices.push_back(n.x);
								m_TranslucentMesh.m_Vertices.push_back(n.y);
								m_TranslucentMesh.m_Vertices.push_back(n.z);
								m_TranslucentMesh.m_Vertices.push_back(uv[3].x);
								m_TranslucentMesh.m_Vertices.push_back(uv[3].y);

								m_TranslucentMesh.m_Indices.push_back(indices[0] + (m_TranslucentMesh.m_NumberOfFaces * 4));
								m_TranslucentMesh.m_Indices.push_back(indices[1] + (m_TranslucentMesh.m_NumberOfFaces * 4));
								m_TranslucentMesh.m_Indices.push_back(indices[2] + (m_TranslucentMesh.m_NumberOfFaces * 4));
								m_TranslucentMesh.m_Indices.push_back(indices[3] + (m_TranslucentMesh.m_NumberOfFaces * 4));
								m_TranslucentMesh.m_Indices.push_back(indices[4] + (m_TranslucentMesh.m_NumberOfFaces * 4));
								m_TranslucentMesh.m_Indices.push_back(indices[5] + (m_TranslucentMesh.m_NumberOfFaces * 4));

								m_TranslucentMesh.m_NumberOfFaces += 1;
							}
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
	int x = position.x > 0.0f ? (int)(position.x / s_DefaultDimensions.x) : (int)((position.x - (s_DefaultDimensions.x - 1)) / s_DefaultDimensions.x);
	int z = position.z > 0.0f ? (int)(position.z / s_DefaultDimensions.z) : (int)((position.z - (s_DefaultDimensions.z - 1)) / s_DefaultDimensions.z);

	return std::pair<int, int>(x, z);
}

int Chunk::GetChunkLayerFromWorld(const glm::vec3& position)
{
	return (int)((position.y - s_DefaultYPosition) / s_DefaultLayerSize);
}

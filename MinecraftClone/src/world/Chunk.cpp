#include "Chunk.h"

glm::ivec3 Chunk::s_DefaultDimensions(16, 128, 16);
int Chunk::s_DefaultYPosition = -64;
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

	throw std::runtime_error("The provided position is out of chunk!");
}

void Chunk::GenerateHeightMap(const NoiseGenerator& generator, int minYPosition)
{
	int chunkWorldPosition[3] = { m_Position.first * s_DefaultDimensions.x, s_DefaultYPosition, m_Position.second * s_DefaultDimensions.z };
	int maxYPosition = s_DefaultDimensions.y;

	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int z = 0; z < s_DefaultDimensions.z; z++)
		{
			float noise = generator.GetNoise2D((float)(x + chunkWorldPosition[0]), (float)(z + chunkWorldPosition[2]));

			m_HeightMap[x][z] = (int)(minYPosition + (((noise + 1.0f) / 2.0f) * maxYPosition));
		}
	}
}

void Chunk::GenerateStructures(const StructuresProfile& configuration)
{
	int chunkWorldPosition[3] = { m_Position.first * s_DefaultDimensions.x, s_DefaultYPosition, m_Position.second * s_DefaultDimensions.z };
	int treesMinHeight = 3, treesMaxHeight = 8; // WARNING: Hard-coded values.
	int seaLevel = s_DefaultYPosition + s_DefaultDimensions.y;

	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int z = 0; z < s_DefaultDimensions.z; z++)
		{
			int maxHeight = m_HeightMap[x][z];
			int y = maxHeight + 1;

			if (maxHeight > seaLevel) // Only spawn structures above the sea level.
			{
				float chanceToSpawnFlora[3] = { // Chances to spawn leaves, flowers and trees.
					(float)std::rand() / RAND_MAX,
					(float)std::rand() / RAND_MAX,
					(float)std::rand() / RAND_MAX
				};

				int xWorldPos = x + chunkWorldPosition[0];
				int yWorldPos = y + chunkWorldPosition[1];
				int zWorldPos = z + chunkWorldPosition[2];

				if (chanceToSpawnFlora[0] <= configuration.m_LeavesFrequency)
				{
					glm::vec3 blockPosition(xWorldPos, yWorldPos, zWorldPos);

					StructuresHandler::InsertBlockAt(m_Position, Block(BlockType::GRASSLEAVES, blockPosition, false, true, true));
				}
				else if (chanceToSpawnFlora[1] <= configuration.m_FlowersFrequency)
				{
					int flowerTypeOffset = (int)(7 * (float)std::rand() / RAND_MAX);
					BlockType blockType = (BlockType)(41 + flowerTypeOffset); // Based in the position of the first flower on texture atlas.
					glm::vec3 blockPosition(xWorldPos, yWorldPos, zWorldPos);

					StructuresHandler::InsertBlockAt(m_Position, Block(blockType, blockPosition, false, true, true));
				}
				else if (chanceToSpawnFlora[2] <= configuration.m_TreesFrequency)
				{
					int treeHeight = (int)(treesMinHeight + ((treesMaxHeight - treesMinHeight) * (float)std::rand() / RAND_MAX));

					for (int i = 0; i < treeHeight; i++)
					{
						glm::vec3 blockPosition(xWorldPos, yWorldPos + i, zWorldPos);

						StructuresHandler::InsertBlockAt(m_Position, Block(BlockType::OAKWOOD, blockPosition));
					}

					for (int i = -2; i <= 2; i++)
					{
						for (int j = -2; j <= 2; j++)
						{
							for (int k = -2; k <= 2; k++)
							{
								switch (j)
								{
								case -2:
									if (i == 0 && k == 0) { continue; }
									else if (i == -2 || i == 2 || k == -2 || k == 2) { continue; }
									break;

								case -1:
									if (i == 0 && k == 0) { continue; }
									else if ((i == -2 && k == -2) || (i == -2 && k == 2) || (i == 2 && k == -2) || (i == 2 && k == 2)) { continue; }
									break;

								case  0:
									if ((i == -2 && k == -2) || (i == -2 && k == 2) || (i == 2 && k == -2) || (i == 2 && k == 2)) { continue; }
									break;

								case  1:
									if (i == -2 || i == 2 || k == -2 || k == 2) { continue; }
									break;

								case  2:
									if (i == -2 || i == 2 || k == -2 || k == 2) { continue; }
									else if ((i == -1 && k == -1) || (i == -1 && k == 1) || (i == 1 && k == -1) || (i == 1 && k == 1)) { continue; }
									break;

								default:
									break;
								}

								glm::vec3 blockPosition(xWorldPos + i, yWorldPos + j + treeHeight, zWorldPos + k);
								std::pair<int, int> chunkPosition = GetChunkPositionFromWorld(blockPosition);

								StructuresHandler::InsertBlockAt(chunkPosition, Block(BlockType::OAKLEAVES, blockPosition, true, true));
							}
						}
					}
				}
			}
		}
	}
}

void Chunk::GenerateBlocks()
{
	int chunkWorldPosition[3] = { m_Position.first * s_DefaultDimensions.x, s_DefaultYPosition, m_Position.second * s_DefaultDimensions.z };
	int seaLevel = s_DefaultYPosition + s_DefaultDimensions.y; // At zero Y coordinate.

	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int y = 0; y < s_DefaultDimensions.y; y++)
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
						blockType = y >= seaLevel ? BlockType::GRASS : BlockType::SAND;
					}
					else
					{
						blockType = y >= maxHeight / 2 ? BlockType::DIRTY : BlockType::STONE;
					}

					InsertBlockAt(Block(blockType, glm::vec3(xWorldPos, yWorldPos, zWorldPos)), glm::ivec3(x, y, z));
				}
				else if (y < seaLevel)
				{
					InsertBlockAt(Block(BlockType::WATER, glm::vec3(xWorldPos, yWorldPos, zWorldPos), false, true), glm::ivec3(x, y, z));
				}
				else if (y > seaLevel)
				{
					// Level of structures generation.
				}
			}
		}
	}

	CheckForGeneratedStructures();
}

void Chunk::CheckForGeneratedStructures()
{
	int chunkWorldPosition[3] = { m_Position.first * s_DefaultDimensions.x, s_DefaultYPosition, m_Position.second * s_DefaultDimensions.z };
	std::vector<Block>* structuresBlocksList = StructuresHandler::GetBlocksListIfExists(m_Position);

	if (structuresBlocksList != nullptr)
	{
		for (const Block& block : *structuresBlocksList)
		{
			int x = (int)block.m_Position.x - chunkWorldPosition[0];
			int y = (int)block.m_Position.y - chunkWorldPosition[1];
			int z = (int)block.m_Position.z - chunkWorldPosition[2];

			InsertBlockAt(block, glm::ivec3(x, y, z));
		}
	}
}

void Chunk::GenerateMesh(Chunk* chunksArround[8])
{
	SampleRenderableFaces(chunksArround);

	m_OpaqueMesh.GenerateRenderData();

	m_TranslucentMesh.GenerateRenderData();
}

void Chunk::UpdateMesh(Chunk* chunksArround[8])
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
	std::vector<BlockType> intersectionBlackList;
	float distanceToClosestBlock = 0.0f;

	intersectionBlackList.push_back(BlockType::EMPTY);
	intersectionBlackList.push_back(BlockType::WATER);
	
	for (int x = 0; x < s_DefaultDimensions.x; x++)
	{
		for (int y = 0; y < s_DefaultDimensions.y; y++)
		{
			for (int z = 0; z < s_DefaultDimensions.z; z++)
			{
				Block& block = m_Blocks[x][y][z];
				bool onBlackList = false;

				for (const BlockType& blockType : intersectionBlackList)
				{
					if (block.GetType() == blockType)
					{
						onBlackList = true;
						break;
					}
				}

				if (!onBlackList)
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

void Chunk::SampleRenderableFaces(Chunk* chunksArround[8])
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
					if (!block.m_SpecialShape)
					{
						for (int faceIndex = 0; faceIndex < 6; faceIndex++) // Iterate over all block faces.
						{
							bool faceCanComposeMesh = false;

							if (faceIndex == BlockFace::FRONT)
							{
								if (localBlockPosition.z == s_DefaultDimensions.z - 1)
								{
									Chunk* chunk = chunksArround[faceIndex];

									if (chunk != nullptr)
									{
										glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Cube::s_Normals[faceIndex]);
										Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

										if (obstructingBlock.GetType() == BlockType::EMPTY)
										{
											faceCanComposeMesh = true;
										}
										else
										{
											if (block.m_Solid && obstructingBlock.m_Translucent)
											{
												faceCanComposeMesh = true;
											}
											else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
											{
												faceCanComposeMesh = true;
											}
										}
									}
									else
									{
										faceCanComposeMesh = true;
									}
								}
								else
								{
									Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[faceIndex]);

									if (obstructingBlock.GetType() == BlockType::EMPTY)
									{
										faceCanComposeMesh = true;
									}
									else
									{
										if (block.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
										else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
									}
								}
							}
							else if (faceIndex == BlockFace::BACK)
							{
								if (localBlockPosition.z == 0)
								{
									Chunk* chunk = chunksArround[faceIndex];

									if (chunk != nullptr)
									{
										glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Cube::s_Normals[faceIndex]);
										Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

										if (obstructingBlock.GetType() == BlockType::EMPTY)
										{
											faceCanComposeMesh = true;
										}
										else
										{
											if (block.m_Solid && obstructingBlock.m_Translucent)
											{
												faceCanComposeMesh = true;
											}
											else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
											{
												faceCanComposeMesh = true;
											}
										}
									}
									else
									{
										faceCanComposeMesh = true;
									}
								}
								else
								{
									Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[faceIndex]);

									if (obstructingBlock.GetType() == BlockType::EMPTY)
									{
										faceCanComposeMesh = true;
									}
									else
									{
										if (block.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
										else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
									}
								}
							}
							else if (faceIndex == BlockFace::RIGHT)
							{
								if (localBlockPosition.x == s_DefaultDimensions.x - 1)
								{
									Chunk* chunk = chunksArround[faceIndex];

									if (chunk != nullptr)
									{
										glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Cube::s_Normals[faceIndex]);
										Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

										if (obstructingBlock.GetType() == BlockType::EMPTY)
										{
											faceCanComposeMesh = true;
										}
										else
										{
											if (block.m_Solid && obstructingBlock.m_Translucent)
											{
												faceCanComposeMesh = true;
											}
											else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
											{
												faceCanComposeMesh = true;
											}
										}
									}
									else
									{
										faceCanComposeMesh = true;
									}
								}
								else
								{
									Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[faceIndex]);

									if (obstructingBlock.GetType() == BlockType::EMPTY)
									{
										faceCanComposeMesh = true;
									}
									else
									{
										if (block.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
										else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
									}
								}
							}
							else if (faceIndex == BlockFace::LEFT)
							{
								if (localBlockPosition.x == 0)
								{
									Chunk* chunk = chunksArround[faceIndex];

									if (chunk != nullptr)
									{
										glm::ivec3 nextPosition = GetNextLocalBlockPosition(localBlockPosition, Cube::s_Normals[faceIndex]);
										Block& obstructingBlock = chunk->GetBlockAt(nextPosition);

										if (obstructingBlock.GetType() == BlockType::EMPTY)
										{
											faceCanComposeMesh = true;
										}
										else
										{
											if (block.m_Solid && obstructingBlock.m_Translucent)
											{
												faceCanComposeMesh = true;
											}
											else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
											{
												faceCanComposeMesh = true;
											}
										}
									}
									else
									{
										faceCanComposeMesh = true;
									}
								}
								else
								{
									Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[faceIndex]);

									if (obstructingBlock.GetType() == BlockType::EMPTY)
									{
										faceCanComposeMesh = true;
									}
									else
									{
										if (block.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
										else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
									}
								}
							}
							else if (faceIndex == BlockFace::TOP)
							{
								if (localBlockPosition.y == s_DefaultDimensions.y - 1)
								{
									faceCanComposeMesh = true;
								}
								else
								{
									Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[faceIndex]);

									if (obstructingBlock.GetType() == BlockType::EMPTY)
									{
										faceCanComposeMesh = true;
									}
									else
									{
										if (block.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
										else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
									}
								}
							}
							else if (faceIndex == BlockFace::BOTTOM)
							{
								if (localBlockPosition.y == 0)
								{
									faceCanComposeMesh = true;
								}
								else
								{
									Block& obstructingBlock = GetBlockAt(localBlockPosition + Cube::s_Normals[faceIndex]);

									if (obstructingBlock.GetType() == BlockType::EMPTY)
									{
										faceCanComposeMesh = true;
									}
									else
									{
										if (block.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
										else if (obstructingBlock.m_Solid && obstructingBlock.m_Translucent)
										{
											faceCanComposeMesh = true;
										}
									}
								}
							}

							if (faceCanComposeMesh)
							{
								glm::vec3 v0 = Cube::s_Vertices[faceIndex][0] + (glm::vec3)localBlockPosition;
								glm::vec3 v1 = Cube::s_Vertices[faceIndex][1] + (glm::vec3)localBlockPosition;
								glm::vec3 v2 = Cube::s_Vertices[faceIndex][2] + (glm::vec3)localBlockPosition;
								glm::vec3 v3 = Cube::s_Vertices[faceIndex][3] + (glm::vec3)localBlockPosition;

								glm::vec3 n = (glm::vec3)Cube::s_Normals[faceIndex];
								std::array<glm::vec2, 4> uv = Block::GenerateTexCoords(block.GetType());

								unsigned int* indices = Cube::s_Indices; // Same indices to all block faces.
								float occlusion = 0.0f; // Vertices ambient occlusion.

								if (!block.m_Translucent) // Opaque mesh.
								{
									for (int vertexIndex = 0; vertexIndex < 4; vertexIndex++) // Calculating occlusion, iterate over the vertices.
									{
										glm::vec3 vertexDirection = glm::sign(Cube::s_Vertices[faceIndex][vertexIndex]);
										glm::vec3 sideBlocksDirections[3];

										switch (faceIndex)
										{
										case BlockFace::FRONT:
										case BlockFace::BACK:
											sideBlocksDirections[0] = vertexDirection * glm::vec3(0.0f, 1.0f, 1.0f);
											sideBlocksDirections[1] = vertexDirection * glm::vec3(1.0f, 0.0f, 1.0f);
											sideBlocksDirections[2] = vertexDirection * glm::vec3(1.0f, 1.0f, 1.0f);
											break;

										case BlockFace::RIGHT:
										case BlockFace::LEFT:
											sideBlocksDirections[0] = vertexDirection * glm::vec3(1.0f, 1.0f, 0.0f);
											sideBlocksDirections[1] = vertexDirection * glm::vec3(1.0f, 0.0f, 1.0f);
											sideBlocksDirections[2] = vertexDirection * glm::vec3(1.0f, 1.0f, 1.0f);
											break;

										case BlockFace::TOP:
										case BlockFace::BOTTOM:
											sideBlocksDirections[0] = vertexDirection * glm::vec3(1.0f, 1.0f, 0.0f);
											sideBlocksDirections[1] = vertexDirection * glm::vec3(0.0f, 1.0f, 1.0f);
											sideBlocksDirections[2] = vertexDirection * glm::vec3(1.0f, 1.0f, 1.0f);
											break;

										default:
											break;
										}

										Block side0Block = GetBlockCopyFromWorld(block.m_Position + sideBlocksDirections[0], chunksArround);
										Block side1Block = GetBlockCopyFromWorld(block.m_Position + sideBlocksDirections[1], chunksArround);
										Block side2Block = GetBlockCopyFromWorld(block.m_Position + sideBlocksDirections[2], chunksArround);

										bool occluded = false;
										occluded |= side0Block.GetType() != BlockType::EMPTY && !side0Block.m_Translucent;
										occluded |= side1Block.GetType() != BlockType::EMPTY && !side1Block.m_Translucent;
										occluded |= side2Block.GetType() != BlockType::EMPTY && !side2Block.m_Translucent;

										if (occluded)
										{
											occlusion = 0.05f; // Corner shadow to simulate ambient occlusion.

											break;
										}
									}
									
									if (block.GetType() == BlockType::GRASS)
									{
										if (faceIndex != BlockFace::TOP)
										{
											if (faceIndex == BlockFace::BOTTOM) { uv = Block::GenerateTexCoords(BlockType::DIRTY); }
											else { uv = Block::GenerateTexCoords(BlockType::SIDE_GRASS); }
										}
									}

									if (block.GetType() == BlockType::OAKWOOD)
									{
										if (faceIndex != BlockFace::TOP && faceIndex != BlockFace::BOTTOM)
										{
											uv = Block::GenerateTexCoords(BlockType::SIDE_OAKWOOD);
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
									m_OpaqueMesh.m_Vertices.push_back(occlusion);

									m_OpaqueMesh.m_Vertices.push_back(v1.x);
									m_OpaqueMesh.m_Vertices.push_back(v1.y);
									m_OpaqueMesh.m_Vertices.push_back(v1.z);
									m_OpaqueMesh.m_Vertices.push_back(n.x);
									m_OpaqueMesh.m_Vertices.push_back(n.y);
									m_OpaqueMesh.m_Vertices.push_back(n.z);
									m_OpaqueMesh.m_Vertices.push_back(uv[1].x);
									m_OpaqueMesh.m_Vertices.push_back(uv[1].y);
									m_OpaqueMesh.m_Vertices.push_back(occlusion);

									m_OpaqueMesh.m_Vertices.push_back(v2.x);
									m_OpaqueMesh.m_Vertices.push_back(v2.y);
									m_OpaqueMesh.m_Vertices.push_back(v2.z);
									m_OpaqueMesh.m_Vertices.push_back(n.x);
									m_OpaqueMesh.m_Vertices.push_back(n.y);
									m_OpaqueMesh.m_Vertices.push_back(n.z);
									m_OpaqueMesh.m_Vertices.push_back(uv[2].x);
									m_OpaqueMesh.m_Vertices.push_back(uv[2].y);
									m_OpaqueMesh.m_Vertices.push_back(occlusion);

									m_OpaqueMesh.m_Vertices.push_back(v3.x);
									m_OpaqueMesh.m_Vertices.push_back(v3.y);
									m_OpaqueMesh.m_Vertices.push_back(v3.z);
									m_OpaqueMesh.m_Vertices.push_back(n.x);
									m_OpaqueMesh.m_Vertices.push_back(n.y);
									m_OpaqueMesh.m_Vertices.push_back(n.z);
									m_OpaqueMesh.m_Vertices.push_back(uv[3].x);
									m_OpaqueMesh.m_Vertices.push_back(uv[3].y);
									m_OpaqueMesh.m_Vertices.push_back(occlusion);

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
									m_TranslucentMesh.m_Vertices.push_back(occlusion);

									m_TranslucentMesh.m_Vertices.push_back(v1.x);
									m_TranslucentMesh.m_Vertices.push_back(v1.y);
									m_TranslucentMesh.m_Vertices.push_back(v1.z);
									m_TranslucentMesh.m_Vertices.push_back(n.x);
									m_TranslucentMesh.m_Vertices.push_back(n.y);
									m_TranslucentMesh.m_Vertices.push_back(n.z);
									m_TranslucentMesh.m_Vertices.push_back(uv[1].x);
									m_TranslucentMesh.m_Vertices.push_back(uv[1].y);
									m_TranslucentMesh.m_Vertices.push_back(occlusion);

									m_TranslucentMesh.m_Vertices.push_back(v2.x);
									m_TranslucentMesh.m_Vertices.push_back(v2.y);
									m_TranslucentMesh.m_Vertices.push_back(v2.z);
									m_TranslucentMesh.m_Vertices.push_back(n.x);
									m_TranslucentMesh.m_Vertices.push_back(n.y);
									m_TranslucentMesh.m_Vertices.push_back(n.z);
									m_TranslucentMesh.m_Vertices.push_back(uv[2].x);
									m_TranslucentMesh.m_Vertices.push_back(uv[2].y);
									m_TranslucentMesh.m_Vertices.push_back(occlusion);

									m_TranslucentMesh.m_Vertices.push_back(v3.x);
									m_TranslucentMesh.m_Vertices.push_back(v3.y);
									m_TranslucentMesh.m_Vertices.push_back(v3.z);
									m_TranslucentMesh.m_Vertices.push_back(n.x);
									m_TranslucentMesh.m_Vertices.push_back(n.y);
									m_TranslucentMesh.m_Vertices.push_back(n.z);
									m_TranslucentMesh.m_Vertices.push_back(uv[3].x);
									m_TranslucentMesh.m_Vertices.push_back(uv[3].y);
									m_TranslucentMesh.m_Vertices.push_back(occlusion);

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
					else
					{
						glm::vec3* fVertices = Cube::s_Vertices[0];
						glm::vec3* bVertices = Cube::s_Vertices[1];

						glm::vec3 v0 = fVertices[0] + (glm::vec3)localBlockPosition;
						glm::vec3 v1 = fVertices[1] + (glm::vec3)localBlockPosition;
						glm::vec3 v2 = fVertices[2] + (glm::vec3)localBlockPosition;
						glm::vec3 v3 = fVertices[3] + (glm::vec3)localBlockPosition;
						glm::vec3 v4 = bVertices[0] + (glm::vec3)localBlockPosition;
						glm::vec3 v5 = bVertices[1] + (glm::vec3)localBlockPosition;
						glm::vec3 v6 = bVertices[2] + (glm::vec3)localBlockPosition;
						glm::vec3 v7 = bVertices[3] + (glm::vec3)localBlockPosition;

						glm::vec3 n0 = glm::normalize(glm::vec3(1.0f, 0.0f,  1.0f));
						glm::vec3 n1 = glm::normalize(glm::vec3(1.0f, 0.0f, -1.0f));
						std::array<glm::vec2, 4> uv = Block::GenerateTexCoords(block.GetType());

						unsigned int* indices = Cube::s_Indices; // Same indices to all block faces.
						float occlusion = 0.0f; // Vertices ambient occlusion.

						if (!block.m_Translucent) // Opaque mesh.
						{
							m_OpaqueMesh.m_Vertices.push_back(v0.x);
							m_OpaqueMesh.m_Vertices.push_back(v0.y);
							m_OpaqueMesh.m_Vertices.push_back(v0.z);
							m_OpaqueMesh.m_Vertices.push_back(n0.x);
							m_OpaqueMesh.m_Vertices.push_back(n0.y);
							m_OpaqueMesh.m_Vertices.push_back(n0.z);
							m_OpaqueMesh.m_Vertices.push_back(uv[0].x);
							m_OpaqueMesh.m_Vertices.push_back(uv[0].y);
							m_OpaqueMesh.m_Vertices.push_back(occlusion);

							m_OpaqueMesh.m_Vertices.push_back(v4.x);
							m_OpaqueMesh.m_Vertices.push_back(v4.y);
							m_OpaqueMesh.m_Vertices.push_back(v4.z);
							m_OpaqueMesh.m_Vertices.push_back(n0.x);
							m_OpaqueMesh.m_Vertices.push_back(n0.y);
							m_OpaqueMesh.m_Vertices.push_back(n0.z);
							m_OpaqueMesh.m_Vertices.push_back(uv[1].x);
							m_OpaqueMesh.m_Vertices.push_back(uv[1].y);
							m_OpaqueMesh.m_Vertices.push_back(occlusion);

							m_OpaqueMesh.m_Vertices.push_back(v7.x);
							m_OpaqueMesh.m_Vertices.push_back(v7.y);
							m_OpaqueMesh.m_Vertices.push_back(v7.z);
							m_OpaqueMesh.m_Vertices.push_back(n0.x);
							m_OpaqueMesh.m_Vertices.push_back(n0.y);
							m_OpaqueMesh.m_Vertices.push_back(n0.z);
							m_OpaqueMesh.m_Vertices.push_back(uv[2].x);
							m_OpaqueMesh.m_Vertices.push_back(uv[2].y);
							m_OpaqueMesh.m_Vertices.push_back(occlusion);

							m_OpaqueMesh.m_Vertices.push_back(v3.x);
							m_OpaqueMesh.m_Vertices.push_back(v3.y);
							m_OpaqueMesh.m_Vertices.push_back(v3.z);
							m_OpaqueMesh.m_Vertices.push_back(n0.x);
							m_OpaqueMesh.m_Vertices.push_back(n0.y);
							m_OpaqueMesh.m_Vertices.push_back(n0.z);
							m_OpaqueMesh.m_Vertices.push_back(uv[3].x);
							m_OpaqueMesh.m_Vertices.push_back(uv[3].y);
							m_OpaqueMesh.m_Vertices.push_back(occlusion);

							m_OpaqueMesh.m_Indices.push_back(indices[0] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[1] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[2] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[3] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[4] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[5] + (m_OpaqueMesh.m_NumberOfFaces * 4));

							m_OpaqueMesh.m_NumberOfFaces += 1;

							m_OpaqueMesh.m_Vertices.push_back(v1.x);
							m_OpaqueMesh.m_Vertices.push_back(v1.y);
							m_OpaqueMesh.m_Vertices.push_back(v1.z);
							m_OpaqueMesh.m_Vertices.push_back(n1.x);
							m_OpaqueMesh.m_Vertices.push_back(n1.y);
							m_OpaqueMesh.m_Vertices.push_back(n1.z);
							m_OpaqueMesh.m_Vertices.push_back(uv[0].x);
							m_OpaqueMesh.m_Vertices.push_back(uv[0].y);
							m_OpaqueMesh.m_Vertices.push_back(occlusion);

							m_OpaqueMesh.m_Vertices.push_back(v5.x);
							m_OpaqueMesh.m_Vertices.push_back(v5.y);
							m_OpaqueMesh.m_Vertices.push_back(v5.z);
							m_OpaqueMesh.m_Vertices.push_back(n1.x);
							m_OpaqueMesh.m_Vertices.push_back(n1.y);
							m_OpaqueMesh.m_Vertices.push_back(n1.z);
							m_OpaqueMesh.m_Vertices.push_back(uv[1].x);
							m_OpaqueMesh.m_Vertices.push_back(uv[1].y);
							m_OpaqueMesh.m_Vertices.push_back(occlusion);

							m_OpaqueMesh.m_Vertices.push_back(v6.x);
							m_OpaqueMesh.m_Vertices.push_back(v6.y);
							m_OpaqueMesh.m_Vertices.push_back(v6.z);
							m_OpaqueMesh.m_Vertices.push_back(n1.x);
							m_OpaqueMesh.m_Vertices.push_back(n1.y);
							m_OpaqueMesh.m_Vertices.push_back(n1.z);
							m_OpaqueMesh.m_Vertices.push_back(uv[2].x);
							m_OpaqueMesh.m_Vertices.push_back(uv[2].y);
							m_OpaqueMesh.m_Vertices.push_back(occlusion);

							m_OpaqueMesh.m_Vertices.push_back(v2.x);
							m_OpaqueMesh.m_Vertices.push_back(v2.y);
							m_OpaqueMesh.m_Vertices.push_back(v2.z);
							m_OpaqueMesh.m_Vertices.push_back(n1.x);
							m_OpaqueMesh.m_Vertices.push_back(n1.y);
							m_OpaqueMesh.m_Vertices.push_back(n1.z);
							m_OpaqueMesh.m_Vertices.push_back(uv[3].x);
							m_OpaqueMesh.m_Vertices.push_back(uv[3].y);
							m_OpaqueMesh.m_Vertices.push_back(occlusion);

							m_OpaqueMesh.m_Indices.push_back(indices[0] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[1] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[2] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[3] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[4] + (m_OpaqueMesh.m_NumberOfFaces * 4));
							m_OpaqueMesh.m_Indices.push_back(indices[5] + (m_OpaqueMesh.m_NumberOfFaces * 4));

							m_OpaqueMesh.m_NumberOfFaces += 1;
						}
						else
						{
							m_TranslucentMesh.m_Vertices.push_back(v0.x);
							m_TranslucentMesh.m_Vertices.push_back(v0.y);
							m_TranslucentMesh.m_Vertices.push_back(v0.z);
							m_TranslucentMesh.m_Vertices.push_back(n0.x);
							m_TranslucentMesh.m_Vertices.push_back(n0.y);
							m_TranslucentMesh.m_Vertices.push_back(n0.z);
							m_TranslucentMesh.m_Vertices.push_back(uv[0].x);
							m_TranslucentMesh.m_Vertices.push_back(uv[0].y);
							m_TranslucentMesh.m_Vertices.push_back(occlusion);

							m_TranslucentMesh.m_Vertices.push_back(v4.x);
							m_TranslucentMesh.m_Vertices.push_back(v4.y);
							m_TranslucentMesh.m_Vertices.push_back(v4.z);
							m_TranslucentMesh.m_Vertices.push_back(n0.x);
							m_TranslucentMesh.m_Vertices.push_back(n0.y);
							m_TranslucentMesh.m_Vertices.push_back(n0.z);
							m_TranslucentMesh.m_Vertices.push_back(uv[1].x);
							m_TranslucentMesh.m_Vertices.push_back(uv[1].y);
							m_TranslucentMesh.m_Vertices.push_back(occlusion);

							m_TranslucentMesh.m_Vertices.push_back(v7.x);
							m_TranslucentMesh.m_Vertices.push_back(v7.y);
							m_TranslucentMesh.m_Vertices.push_back(v7.z);
							m_TranslucentMesh.m_Vertices.push_back(n0.x);
							m_TranslucentMesh.m_Vertices.push_back(n0.y);
							m_TranslucentMesh.m_Vertices.push_back(n0.z);
							m_TranslucentMesh.m_Vertices.push_back(uv[2].x);
							m_TranslucentMesh.m_Vertices.push_back(uv[2].y);
							m_TranslucentMesh.m_Vertices.push_back(occlusion);

							m_TranslucentMesh.m_Vertices.push_back(v3.x);
							m_TranslucentMesh.m_Vertices.push_back(v3.y);
							m_TranslucentMesh.m_Vertices.push_back(v3.z);
							m_TranslucentMesh.m_Vertices.push_back(n0.x);
							m_TranslucentMesh.m_Vertices.push_back(n0.y);
							m_TranslucentMesh.m_Vertices.push_back(n0.z);
							m_TranslucentMesh.m_Vertices.push_back(uv[3].x);
							m_TranslucentMesh.m_Vertices.push_back(uv[3].y);
							m_TranslucentMesh.m_Vertices.push_back(occlusion);

							m_TranslucentMesh.m_Indices.push_back(indices[0] + (m_TranslucentMesh.m_NumberOfFaces * 4));
							m_TranslucentMesh.m_Indices.push_back(indices[1] + (m_TranslucentMesh.m_NumberOfFaces * 4));
							m_TranslucentMesh.m_Indices.push_back(indices[2] + (m_TranslucentMesh.m_NumberOfFaces * 4));
							m_TranslucentMesh.m_Indices.push_back(indices[3] + (m_TranslucentMesh.m_NumberOfFaces * 4));
							m_TranslucentMesh.m_Indices.push_back(indices[4] + (m_TranslucentMesh.m_NumberOfFaces * 4));
							m_TranslucentMesh.m_Indices.push_back(indices[5] + (m_TranslucentMesh.m_NumberOfFaces * 4));

							m_TranslucentMesh.m_NumberOfFaces += 1;

							m_TranslucentMesh.m_Vertices.push_back(v1.x);
							m_TranslucentMesh.m_Vertices.push_back(v1.y);
							m_TranslucentMesh.m_Vertices.push_back(v1.z);
							m_TranslucentMesh.m_Vertices.push_back(n1.x);
							m_TranslucentMesh.m_Vertices.push_back(n1.y);
							m_TranslucentMesh.m_Vertices.push_back(n1.z);
							m_TranslucentMesh.m_Vertices.push_back(uv[0].x);
							m_TranslucentMesh.m_Vertices.push_back(uv[0].y);
							m_TranslucentMesh.m_Vertices.push_back(occlusion);

							m_TranslucentMesh.m_Vertices.push_back(v5.x);
							m_TranslucentMesh.m_Vertices.push_back(v5.y);
							m_TranslucentMesh.m_Vertices.push_back(v5.z);
							m_TranslucentMesh.m_Vertices.push_back(n1.x);
							m_TranslucentMesh.m_Vertices.push_back(n1.y);
							m_TranslucentMesh.m_Vertices.push_back(n1.z);
							m_TranslucentMesh.m_Vertices.push_back(uv[1].x);
							m_TranslucentMesh.m_Vertices.push_back(uv[1].y);
							m_TranslucentMesh.m_Vertices.push_back(occlusion);

							m_TranslucentMesh.m_Vertices.push_back(v6.x);
							m_TranslucentMesh.m_Vertices.push_back(v6.y);
							m_TranslucentMesh.m_Vertices.push_back(v6.z);
							m_TranslucentMesh.m_Vertices.push_back(n1.x);
							m_TranslucentMesh.m_Vertices.push_back(n1.y);
							m_TranslucentMesh.m_Vertices.push_back(n1.z);
							m_TranslucentMesh.m_Vertices.push_back(uv[2].x);
							m_TranslucentMesh.m_Vertices.push_back(uv[2].y);
							m_TranslucentMesh.m_Vertices.push_back(occlusion);

							m_TranslucentMesh.m_Vertices.push_back(v2.x);
							m_TranslucentMesh.m_Vertices.push_back(v2.y);
							m_TranslucentMesh.m_Vertices.push_back(v2.z);
							m_TranslucentMesh.m_Vertices.push_back(n1.x);
							m_TranslucentMesh.m_Vertices.push_back(n1.y);
							m_TranslucentMesh.m_Vertices.push_back(n1.z);
							m_TranslucentMesh.m_Vertices.push_back(uv[3].x);
							m_TranslucentMesh.m_Vertices.push_back(uv[3].y);
							m_TranslucentMesh.m_Vertices.push_back(occlusion);

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

Block Chunk::GetBlockCopyFromWorld(const glm::vec3& position, Chunk* chunksArround[8])
{
	std::pair<int, int> chunkPosition = GetChunkPositionFromWorld(position);
	int chunkWorldPosition[3] = { chunkPosition.first * s_DefaultDimensions.x, s_DefaultYPosition, chunkPosition.second * s_DefaultDimensions.z };
	glm::ivec3 localBlockPosition((int)position.x - chunkWorldPosition[0], (int)position.y - chunkWorldPosition[1], (int)position.z - chunkWorldPosition[2]);

	if (IsAValidPosition(localBlockPosition))
	{
		if (chunkPosition == m_Position)
		{
			return GetBlockAt(localBlockPosition);
		}
		else
		{
			for (int chunkIndex = 0; chunkIndex < 8; chunkIndex++)
			{
				Chunk* chunk = chunksArround[chunkIndex];

				if (chunk != nullptr)
				{
					if (chunkPosition == chunk->m_Position)
					{
						return chunk->GetBlockAt(localBlockPosition);
					}
				}
			}
		}
	}

	return Block();
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

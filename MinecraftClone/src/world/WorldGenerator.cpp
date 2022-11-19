#include "WorldGenerator.h"

FastNoiseLite WorldGenerator::s_NoiseGenerator;

void WorldGenerator::Execute(World* world, const std::pair<int, int>& origin, int stride)
{
	unsigned int minYPosition = 8;
	unsigned int maxYPosition = Chunk::s_DefaultDimensions.y / 2;

	s_NoiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	s_NoiseGenerator.SetSeed(std::rand());

	// FIXME: Generate noise values before iterate over the blocks.

	for (int i = origin.first - stride; i <= origin.first + stride; i++)
	{
		for (int j = origin.second - stride; j <= origin.second + stride; j++)
		{
			std::pair<int, int> chunkPosition(i, j);
			Chunk chunk(chunkPosition);

			for (unsigned int x = 0; x < Chunk::s_DefaultDimensions.x; x++)
			{
				for (unsigned int y = 0; y < maxYPosition; y++)
				{
					for (unsigned int z = 0; z < Chunk::s_DefaultDimensions.z; z++)
					{
						int xWorldPos = x + (i * Chunk::s_DefaultDimensions.x);
						int yWorldPos = y + Chunk::s_DefaultYPosition;
						int zWorldPos = z + (j * Chunk::s_DefaultDimensions.z);
						float noise = s_NoiseGenerator.GetNoise((float)xWorldPos, (float)zWorldPos);
						unsigned int maxHeight = minYPosition + ((noise + 1.0f) / 2.0f) * maxYPosition;

						if (y <= maxHeight)
						{
							glm::ivec3 localBlockPosition(x, y, z);
							
							chunk.InsertBlockAt(Block(Block::Type::DIRTY, glm::vec3(xWorldPos, yWorldPos, zWorldPos)), localBlockPosition);
						}
					}
				}
			}

			world->InsertChunk(chunk);
		}
	}

	world->GenerateMeshes();
}

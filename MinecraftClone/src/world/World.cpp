#include "World.h"

World::World()
{
}

World::~World()
{
}

void World::InsertChunk(const Chunk& chunk)
{
	m_Chunks.insert({ chunk.m_Position, chunk });
}

void World::RemoveChunkAt(const std::pair<int, int>& position)
{
	m_Chunks.erase(position);
}

void World::InsertBlockAt(const std::pair<int, int>& chunkPosition, const Block& block, const glm::ivec3& blockPosition)
{
	m_Chunks.at(chunkPosition).InsertBlockAt(block, blockPosition);
}

void World::RemoveBlockAt(const std::pair<int, int>& chunkPosition, const glm::ivec3& blockPosition)
{
	m_Chunks.at(chunkPosition).RemoveBlockAt(blockPosition);
}

Chunk& World::GetChunkAt(const std::pair<int, int>& position)
{
	return m_Chunks.at(position);
}

void World::GenerateMeshes()
{
	std::map<std::pair<int, int>, Chunk>::iterator it;

	for (it = m_Chunks.begin(); it != m_Chunks.end(); it++)
	{
		Chunk* chunksArround[4];

		std::pair<int, int> p0 = { it->second.m_Position.first, it->second.m_Position.second + 1 }; // front
		std::pair<int, int> p1 = { it->second.m_Position.first, it->second.m_Position.second - 1 }; // back
		std::pair<int, int> p2 = { it->second.m_Position.first + 1, it->second.m_Position.second }; // right
		std::pair<int, int> p3 = { it->second.m_Position.first - 1, it->second.m_Position.second }; // left

		chunksArround[0] = GetChunkIfExists(p0);
		chunksArround[1] = GetChunkIfExists(p1);
		chunksArround[2] = GetChunkIfExists(p2);
		chunksArround[3] = GetChunkIfExists(p3);

		it->second.GenerateMesh(chunksArround);
	}
}

void World::UpdateChunkMesh(const std::pair<int, int>& chunkPosition)
{
	Chunk& chunk = m_Chunks.at(chunkPosition);
	Chunk* chunksArround[4];

	std::pair<int, int> p0 = { chunk.m_Position.first, chunk.m_Position.second + 1 }; // front
	std::pair<int, int> p1 = { chunk.m_Position.first, chunk.m_Position.second - 1 }; // back
	std::pair<int, int> p2 = { chunk.m_Position.first + 1, chunk.m_Position.second }; // right
	std::pair<int, int> p3 = { chunk.m_Position.first - 1, chunk.m_Position.second }; // left

	chunksArround[0] = GetChunkIfExists(p0);
	chunksArround[1] = GetChunkIfExists(p1);
	chunksArround[2] = GetChunkIfExists(p2);
	chunksArround[3] = GetChunkIfExists(p3);

	chunk.UpdateMesh(chunksArround);
}

void World::Render(Shader* shaderProgram)
{
	// WARNING:
	// 
	// Iterate a map isn't the most performative solution.
	//
	std::map<std::pair<int, int>, Chunk>::iterator it;

	for (it = m_Chunks.begin(); it != m_Chunks.end(); it++)
	{
		it->second.Render(shaderProgram);
	}
}

Intersection World::CastRay(const glm::vec3& origin, const glm::vec3& direction, float length)
{
	Intersection worldIntersection = std::make_tuple(false, std::make_pair(0, 0), glm::ivec3(0), glm::vec3(0.0f), -1);
	float distanceToClosestBlock = 0.0f;

	Ray ray(origin, direction, length);

	int chunkXPos = origin.x / Chunk::s_DefaultDimensions.x;
	int chunkZPos = origin.z / Chunk::s_DefaultDimensions.z;

	std::pair<int, int> p0 = { chunkXPos, chunkZPos }; // Position of the current chunk.
	std::pair<int, int> p1 = { p0.first - 1, p0.second + 1 };
	std::pair<int, int> p2 = { p0.first, p0.second + 1 };
	std::pair<int, int> p3 = { p0.first + 1, p0.second + 1 };
	std::pair<int, int> p4 = { p0.first - 1, p0.second };
	std::pair<int, int> p5 = { p0.first + 1, p0.second };
	std::pair<int, int> p6 = { p0.first - 1, p0.second - 1 };
	std::pair<int, int> p7 = { p0.first, p0.second - 1 };
	std::pair<int, int> p8 = { p0.first + 1, p0.second - 1 };

	Chunk* chunksArround[9];

	chunksArround[0] = GetChunkIfExists(p0);
	chunksArround[1] = GetChunkIfExists(p1);
	chunksArround[2] = GetChunkIfExists(p2);
	chunksArround[3] = GetChunkIfExists(p3);
	chunksArround[4] = GetChunkIfExists(p4);
	chunksArround[5] = GetChunkIfExists(p5);
	chunksArround[6] = GetChunkIfExists(p6);
	chunksArround[7] = GetChunkIfExists(p7);
	chunksArround[8] = GetChunkIfExists(p8);

	for (unsigned int i = 0; i < 9; i++)
	{
		if (chunksArround[i] != nullptr)
		{
			Intersection chunkIntersection = chunksArround[i]->Intersect(ray);

			if (std::get<0>(chunkIntersection))
			{
				float distanceToCurrChunkBlock = glm::length(std::get<3>(chunkIntersection) - ray.m_Origin);

				if (!std::get<0>(worldIntersection) || distanceToCurrChunkBlock < distanceToClosestBlock)
				{
					distanceToClosestBlock = distanceToCurrChunkBlock;
					worldIntersection = chunkIntersection;
				}
			}
		}
	}

	return worldIntersection;
}

Chunk* World::GetChunkIfExists(const std::pair<int, int>& position)
{
	std::map<std::pair<int, int>, Chunk>::iterator it = m_Chunks.find(position);

	if (it != m_Chunks.end())
	{
		return &it->second;
	}

	return nullptr;
}

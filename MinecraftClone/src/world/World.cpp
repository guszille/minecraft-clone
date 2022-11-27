#include "World.h"

World::World(int seed)
	: m_Seed(seed)
{
}

World::~World()
{
}

int World::GetSeed()
{
	return m_Seed;
}

void World::InsertChunk(const Chunk& chunk)
{
	m_Chunks.insert({ chunk.m_Position, chunk });
}

void World::RemoveChunk(const std::pair<int, int>& position)
{
	m_Chunks.erase(position);
}

bool World::InsertBlockAt(const std::pair<int, int>& chunkPosition, const Block& block, const glm::ivec3& blockPosition)
{
	Chunk* chunk = GetChunkIfExists(chunkPosition);

	if (chunk != nullptr)
	{
		return chunk->InsertBlockAt(block, blockPosition);
	}

	return false;
}

bool World::RemoveBlockAt(const std::pair<int, int>& chunkPosition, const glm::ivec3& blockPosition)
{
	Chunk* chunk = GetChunkIfExists(chunkPosition);

	if (chunk != nullptr)
	{
		return chunk->RemoveBlockAt(blockPosition);
	}

	return false;
}

void World::Setup(const std::pair<int, int>& origin, int stride)
{
	for (int i = origin.first - stride; i <= origin.first + stride; i++)
	{
		for (int j = origin.second - stride; j <= origin.second + stride; j++)
		{
			std::pair<int, int> chunkPosition(i, j);
			Chunk chunk(chunkPosition);

			chunk.GenerateHeightMap();
			chunk.GenerateBlocks();

			InsertChunk(chunk);
		}
	}

	GenerateMeshes();

	/*
		Using multithreading to generate the current chunk. 
	
			std::map<std::pair<int, int>, Chunk>::iterator it;
			std::vector<std::thread> threads;

			auto GenerateChunkFn = [](Chunk* chunk)
			{
				chunk->GenerateHeightMap();
				chunk->GenerateBlocks();
			};

			for (int i = origin.first - stride; i <= origin.first + stride; i++)
			{
				for (int j = origin.second - stride; j <= origin.second + stride; j++)
				{
					InsertChunk(Chunk({ i, j }));
				}
			}

			for (it = m_Chunks.begin(); it != m_Chunks.end(); it++)
			{
				threads.push_back(std::thread(GenerateChunkFn, &it->second));
			}

			for (int i = 0; i < threads.size(); i++)
			{
				threads[i].join();
			}

			GenerateMeshes();
	*/
}

void World::Update(const std::pair<int, int>& origin, int stride)
{
	for (int i = origin.first - stride; i <= origin.first + stride; i++)
	{
		for (int j = origin.second - stride; j <= origin.second + stride; j++)
		{
			std::pair<int, int> chunkPosition(i, j);
			Chunk* chunkReference = GetChunkIfExists(chunkPosition);

			if (chunkReference == nullptr)
			{
				Chunk* chunksArround[4];

				std::pair<int, int> p0 = { chunkPosition.first, chunkPosition.second + 1 }; // front
				std::pair<int, int> p1 = { chunkPosition.first, chunkPosition.second - 1 }; // back
				std::pair<int, int> p2 = { chunkPosition.first + 1, chunkPosition.second }; // right
				std::pair<int, int> p3 = { chunkPosition.first - 1, chunkPosition.second }; // left

				chunksArround[0] = GetChunkIfExists(p0);
				chunksArround[1] = GetChunkIfExists(p1);
				chunksArround[2] = GetChunkIfExists(p2);
				chunksArround[3] = GetChunkIfExists(p3);

				Chunk chunk(chunkPosition);

				chunk.GenerateHeightMap();
				chunk.GenerateBlocks();
				chunk.GenerateMesh(chunksArround);

				InsertChunk(chunk);

				UpdateChunkMesh(p0);
				UpdateChunkMesh(p1);
				UpdateChunkMesh(p2);
				UpdateChunkMesh(p3);
			}
		}
	}
}

void World::Render(Shader* shaderProgram)
{
	// WARNING:
	// 
	// Iterate a map isn't the most performative solution.
	//
	std::map<std::pair<int, int>, Chunk>::iterator it;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	for (it = m_Chunks.begin(); it != m_Chunks.end(); it++)
	{
		it->second.Render(shaderProgram);
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
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
	Chunk* chunk = GetChunkIfExists(chunkPosition);

	if (chunk)
	{
		Chunk* chunksArround[4];

		std::pair<int, int> p0 = { chunk->m_Position.first, chunk->m_Position.second + 1 }; // front
		std::pair<int, int> p1 = { chunk->m_Position.first, chunk->m_Position.second - 1 }; // back
		std::pair<int, int> p2 = { chunk->m_Position.first + 1, chunk->m_Position.second }; // right
		std::pair<int, int> p3 = { chunk->m_Position.first - 1, chunk->m_Position.second }; // left

		chunksArround[0] = GetChunkIfExists(p0);
		chunksArround[1] = GetChunkIfExists(p1);
		chunksArround[2] = GetChunkIfExists(p2);
		chunksArround[3] = GetChunkIfExists(p3);

		chunk->UpdateMesh(chunksArround);
	}
}

Intersection World::CastRay(const glm::vec3& origin, const glm::vec3& direction, float length)
{
	Intersection worldIntersection = std::make_tuple(false, std::make_pair(0, 0), glm::ivec3(0), glm::vec3(0.0f), -1);
	float distanceToClosestBlock = 0.0f;

	Ray ray(origin, direction, length);

	std::pair<int, int> p0 = Chunk::GetChunkPositionFromWorld(origin);
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

bool World::CheckCollision(const AABB& aabb, float maxRange)
{
	std::pair<int, int> p0 = Chunk::GetChunkPositionFromWorld(aabb.m_Origin);
	std::pair<int, int> p1 = { p0.first, p0.second + 1 };
	std::pair<int, int> p2 = { p0.first, p0.second - 1 };
	std::pair<int, int> p3 = { p0.first + 1, p0.second };
	std::pair<int, int> p4 = { p0.first - 1, p0.second };

	Chunk* chunksArround[5];

	chunksArround[0] = GetChunkIfExists(p0);
	chunksArround[1] = GetChunkIfExists(p1);
	chunksArround[2] = GetChunkIfExists(p2);
	chunksArround[3] = GetChunkIfExists(p3);
	chunksArround[4] = GetChunkIfExists(p4);

	for (unsigned int i = 0; i < 5; i++)
	{
		if (chunksArround[i] != nullptr)
		{
			if (chunksArround[i]->CheckCollision(aabb, maxRange))
			{
				return true;
			}
		}
	}

	return false;
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

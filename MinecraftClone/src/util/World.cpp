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

void World::InsertChunkBlock(const Chunk& chunk, const Block& block)
{
	m_Chunks.at(chunk.m_Position).InsertBlock(block);
}

void World::RemoveChunkBlockAt(const Chunk& chunk, const glm::ivec3& position)
{
	m_Chunks.at(chunk.m_Position).RemoveBlockAt(position);
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

Chunk* World::GetChunkIfExists(const std::pair<int, int>& position)
{
	std::map<std::pair<int, int>, Chunk>::iterator it = m_Chunks.find(position);

	if (it != m_Chunks.end())
	{
		return &it->second;
	}

	return nullptr;
}

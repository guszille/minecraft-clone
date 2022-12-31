#include "StructuresHandler.h"

std::map<std::pair<int, int>, std::vector<Block>> StructuresHandler::s_Structures;

StructuresProfile::StructuresProfile(float leavesFrequency, float treesFrequency)
	: m_LeavesFrequency(leavesFrequency), m_TreesFrequency(treesFrequency)
{
}

StructuresProfile::~StructuresProfile()
{
}

std::vector<Block>* StructuresHandler::GetBlocksListIfExists(const std::pair<int, int>& position)
{
	std::map<std::pair<int, int>, std::vector<Block>>::iterator it = s_Structures.find(position);

	if (it != s_Structures.end())
	{
		return &it->second;
	}

	return nullptr;
}

void StructuresHandler::InsertBlockAt(const std::pair<int, int>& position, const Block& block)
{
	std::vector<Block>* blocks = GetBlocksListIfExists(position);

	if (blocks != nullptr)
	{
		blocks->push_back(block);
	}
	else
	{
		s_Structures.insert({ position, std::vector<Block>{ block } });
	}
}

#pragma once

#include <map>
#include <vector>

#include "Block.h"

struct StructuresProfile
{
public:
	StructuresProfile(float leavesFrequency, float flowersFrequency, float treesFrequency);
	~StructuresProfile();

	float m_LeavesFrequency, m_FlowersFrequency, m_TreesFrequency;

private:
	// Nothing.
};

class StructuresHandler
{
public:
	static std::vector<Block>* GetBlocksListIfExists(const std::pair<int, int>& position);
	static void InsertBlockAt(const std::pair<int, int>& position, const Block& block);

private:
	StructuresHandler() = delete;
	~StructuresHandler() = delete;

	static std::map<std::pair<int, int>, std::vector<Block>> s_Structures;
};

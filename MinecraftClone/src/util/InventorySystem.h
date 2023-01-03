#pragma once

#include <vector>
#include <string>

#include "../world/Block.h"

class InventorySystem
{
public:
	InventorySystem();
	~InventorySystem();

	void InsertBlockAtEnd(const Block& block, const std::string& blockName);

	void SelectNextSlot();
	void SelectPreviousSlot();

	Block GetBlockOnSelectedSlot();
	std::string GetBlockNameOnSelectedSlot();

private:
	std::vector<std::tuple<Block, std::string>> m_Slots;

	int m_SelectedSlot;
};

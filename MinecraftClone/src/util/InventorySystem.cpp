#include "InventorySystem.h"

InventorySystem::InventorySystem()
	: m_SelectedSlot()
{
}

InventorySystem::~InventorySystem()
{
}

void InventorySystem::InsertBlockAtEnd(const Block& block, const std::string& blockName)
{
	m_Slots.push_back({ block, blockName });
}

void InventorySystem::SelectNextSlot()
{
	int numberOfUsedSlots = (int)m_Slots.size();

	m_SelectedSlot = numberOfUsedSlots > 0 ? (m_SelectedSlot + 1) % numberOfUsedSlots : m_SelectedSlot;
}

void InventorySystem::SelectPreviousSlot()
{
	int numberOfUsedSlots = (int)m_Slots.size();

	m_SelectedSlot = m_SelectedSlot - 1 >= 0 ? m_SelectedSlot - 1 : numberOfUsedSlots - 1;
}

Block InventorySystem::GetBlockOnSelectedSlot()
{
	if (m_Slots.size() > 0)
	{
		return std::get<0>(m_Slots.at(m_SelectedSlot));
	}

	return Block();
}

std::string InventorySystem::GetBlockNameOnSelectedSlot()
{
	if (m_Slots.size() > 0)
	{
		return std::get<1>(m_Slots.at(m_SelectedSlot));
	}

	return "Empty";
}

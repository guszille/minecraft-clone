#pragma once

#include <fnl/FastNoiseLite.h>

#include "World.h"

class WorldGenerator
{
public:
	static void Execute(World* world, const std::pair<int, int>& origin, int stride);

private:
	WorldGenerator() = delete; // Disable instantiating this class.
	~WorldGenerator() = delete;

	static FastNoiseLite s_NoiseGenerator;
};

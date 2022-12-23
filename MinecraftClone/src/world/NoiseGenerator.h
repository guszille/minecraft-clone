#pragma once

#include <fnl/FastNoiseLite.h>

class NoiseGenerator
{
public:
	static void Configure(int seed, float frequency);

	static float GetNoise2D(float x, float y);
	static float GetNoise3D(float x, float y, float z);

private:
	NoiseGenerator() = delete; // Disable instantiating this class.
	~NoiseGenerator() = delete;

	static FastNoiseLite s_NoiseGenerator;
};

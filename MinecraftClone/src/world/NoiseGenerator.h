#pragma once

#include <fnl/FastNoiseLite.h>

class NoiseGenerator
{
public:
	NoiseGenerator(int seed, float frequency, int noiseType = 3, int fractalType = 1, int octaves = 4);
	~NoiseGenerator();

	float GetNoise2D(float x, float y);
	float GetNoise3D(float x, float y, float z);

	float GetNoise2D(float x, float y) const;
	float GetNoise3D(float x, float y, float z) const;

private:
	FastNoiseLite m_Generator;
};

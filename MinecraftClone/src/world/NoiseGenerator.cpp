#include "NoiseGenerator.h"

FastNoiseLite NoiseGenerator::s_NoiseGenerator;

void NoiseGenerator::Configure(int seed)
{
	s_NoiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	s_NoiseGenerator.SetSeed(seed);
}

float NoiseGenerator::GetNoise2D(float x, float y)
{
	return s_NoiseGenerator.GetNoise(x, y);
}

float NoiseGenerator::GetNoise3D(float x, float y, float z)
{
	return s_NoiseGenerator.GetNoise(x, y, z);
}

#include "NoiseGenerator.h"

FastNoiseLite NoiseGenerator::s_NoiseGenerator;

void NoiseGenerator::Configure(int seed, float frequency)
{
	s_NoiseGenerator.SetSeed(seed);
	s_NoiseGenerator.SetFrequency(frequency);

	s_NoiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	s_NoiseGenerator.SetFractalType(FastNoiseLite::FractalType_FBm);
	s_NoiseGenerator.SetFractalOctaves(4);
}

float NoiseGenerator::GetNoise2D(float x, float y)
{
	return s_NoiseGenerator.GetNoise(x, y);
}

float NoiseGenerator::GetNoise3D(float x, float y, float z)
{
	return s_NoiseGenerator.GetNoise(x, y, z);
}

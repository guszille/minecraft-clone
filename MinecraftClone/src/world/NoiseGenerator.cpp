#include "NoiseGenerator.h"

NoiseGenerator::NoiseGenerator(int seed, float frequency, int noiseType, int fractalType, int octaves)
	: m_Generator()
{
	m_Generator.SetSeed(seed);
	m_Generator.SetFrequency(frequency);

	m_Generator.SetNoiseType(FastNoiseLite::NoiseType(noiseType)); // Default is "Perlim".
	m_Generator.SetFractalType(FastNoiseLite::FractalType(fractalType)); // Default is "FBm".
	m_Generator.SetFractalOctaves(octaves);
}

NoiseGenerator::~NoiseGenerator()
{
}

float NoiseGenerator::GetNoise2D(float x, float y)
{
	return m_Generator.GetNoise(x, y);
}

float NoiseGenerator::GetNoise3D(float x, float y, float z)
{
	return m_Generator.GetNoise(x, y, z);
}

float NoiseGenerator::GetNoise2D(float x, float y) const
{
	return m_Generator.GetNoise(x, y);
}

float NoiseGenerator::GetNoise3D(float x, float y, float z) const
{
	return m_Generator.GetNoise(x, y, z);
}

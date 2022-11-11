#pragma once

#include <vector>

#include <glad/glad.h>

class Mesh
{
public:
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;

	unsigned int m_NumberOfFaces;

public:
	Mesh();
	~Mesh();

	void GenerateRenderData();
	void UpdateRenderData();

	void Render();

private:
	unsigned int m_VAO, m_VBO, m_EBO;
};

#pragma once

#include <iostream>

#include <glad/glad.h>

class DepthMap
{
public:
	DepthMap(int width, int height);
	~DepthMap();

	void Bind();
	void Unbind();

	void BindDepthBuffer(int unit);

private:
	unsigned int m_ID, m_DepthBufferID;
};

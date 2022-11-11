#pragma once

#include <iostream>

#include <glad/glad.h>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stbi/stb_image.h>
#endif // _STB_IMAGE_INCLUDED

class Texture
{
public:
	Texture(const char* filepath, const bool gammaCorrection = false);
	~Texture();

	void Bind(int unit);
	void Unbind();

private:
	unsigned int m_ID;
	int m_Width, m_Height, m_ColorChannels;
};

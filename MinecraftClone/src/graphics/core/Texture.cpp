#include "Texture.h"

Texture::Texture(const char* filepath, const bool gammaCorrection)
	: m_ID(), m_Width(), m_Height(), m_ColorChannels()
{
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(filepath, &m_Width, &m_Height, &m_ColorChannels, 0);
	int internalFormat = GL_RED, format = GL_RED; // Default format.

	// WARNING: We are only expecting an image with 3 or 4 color channels.
	//			Any other format may generate some OpenGL error.

	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	switch (m_ColorChannels)
	{
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
		format = GL_RGB;

		break;

	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
		format = GL_RGBA;

		break;

	default:
		std::cout << "[ERROR] TEXTURE: Texture format not supported." << std::endl;

		break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// WARNING!
	// 
	// Defining the index of the highest defined mipmap level to prevent texture artifacts.
	// This is not a final solution, some artifacts remain.
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "[ERROR] TEXTURE: Failed to load texture in \"" << filepath << "\"." << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}

Texture::~Texture()
{
}

void Texture::Bind(int unit)
{
	if (unit >= 0 && unit <= 15)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}
	else
	{
		std::cout << "[ERROR] TEXTURE: Failed to bind texture in " << unit << " unit." << std::endl;
	}
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::ivec3 Texture::GetSize()
{
	return glm::ivec3(m_Width, m_Height, m_ColorChannels);
}

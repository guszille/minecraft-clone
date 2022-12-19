#include "DepthMap.h"

DepthMap::DepthMap(int width, int height)
	: m_ID(), m_DepthBufferID()
{
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	glGenTextures(1, &m_DepthBufferID);
	glBindTexture(GL_TEXTURE_2D, m_DepthBufferID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthBufferID, 0);

	// A framebuffer object however is not complete without a color buffer so we need to explicitly tell OpenGL we're not going to render any color data.
	// We do this by setting both the read and draw buffer to GL_NONE with glDrawBuffer and glReadbuffer.
	//
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DepthMap::~DepthMap()
{
}

void DepthMap::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void DepthMap::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthMap::BindDepthBuffer(int unit)
{
	if (unit >= 0 && unit <= 15)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_DepthBufferID);
	}
	else
	{
		std::cout << "[ERROR] DEPTHMAP: Failed to bind texture in " << unit << " unit." << std::endl;
	}
}

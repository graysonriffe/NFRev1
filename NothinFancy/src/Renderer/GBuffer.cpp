#include "GBuffer.h"

#include "GL/glew.h"

#include "Application.h"
#include "Entity.h"

namespace nf {
	GBuffer::GBuffer() :
		m_FBO(0),
		m_depth(0),
		m_width(0),
		m_height(0)
	{
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		m_width = Application::getApp()->getConfig().width;
		m_height = Application::getApp()->getConfig().height;
		glGenTextures(m_textures.size(), &m_textures[0]);
		for (unsigned int i = 0; i < m_textures.size(); i++) {
			glBindTexture(GL_TEXTURE_2D, m_textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
		}
		glGenTextures(1, &m_depth);
		glBindTexture(GL_TEXTURE_2D, m_depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);
		GLenum draw[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
		glDrawBuffers(m_textures.size(), draw);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GBuffer::resize() {
		unsigned int width, height;
		width = Application::getApp()->getConfig().width;
		height = Application::getApp()->getConfig().height;
		if (m_width != width || m_height != height) {
			m_width = width;
			m_height = height;
			for (unsigned int curr : m_textures) {
				glBindTexture(GL_TEXTURE_2D, curr);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
			}
		}
	}

	void GBuffer::render(std::vector<Entity*>& entites) {

	}

	void GBuffer::bindTextures() {
		for (unsigned int i = 0; i < m_textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		}
	}

	GBuffer::~GBuffer() {
		for (unsigned int curr : m_textures)
			glDeleteTextures(1, &curr);
		glDeleteTextures(1, &m_depth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &m_FBO);
	}
}
#include "GBuffer.h"

#include "GL/glew.h"

#include "Application.h"
#include "Entity.h"
#include "Shader.h"

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
		glGenTextures(1, &m_textures[0]);
		glGenTextures(1, &m_textures[1]);
		glGenTextures(1, &m_textures[2]);
		glGenTextures(1, &m_textures[3]);
		for (unsigned int i = 0; i < m_textures.size(); i++) {
			glBindTexture(GL_TEXTURE_2D, m_textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
		}
		unsigned int draw[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(m_textures.size(), draw);
		glGenRenderbuffers(1, &m_depth);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GBuffer::render(std::vector<Entity*>& entites, Shader* shader) {
		int prevFBO;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		resize();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_BLEND);

		for (Entity* curr : entites)
			curr->render(shader);
		glEnable(GL_BLEND);

		//TODO: Blit depth buffer for transparent objects later

		glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
	}

	void GBuffer::bindTextures(Shader* shader) {
		for (unsigned int i = 0; i < m_textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		}
		shader->setUniform("gBPos", 0);
		shader->setUniform("gBNorm", 1);
		shader->setUniform("gBDiff", 2);
		shader->setUniform("gBSpec", 3);
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
			glDeleteRenderbuffers(1, &m_depth);
			glGenRenderbuffers(1, &m_depth);
			glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);
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
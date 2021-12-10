#include "nf/Texture.h"

#include "GL/glew.h"

#include "nf/Assets.h"
#include "nf/Utility.h"

namespace nf {
	Texture::Texture(ATexture* tex, bool linear) :
		m_isBase(false),
		m_id(0),
		m_x(0),
		m_y(0)
	{
		//Load dimensions and channels from cooked data
		int nChannels;
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, linear ? GL_RGBA : GL_SRGB_ALPHA, m_x, m_y, 0, nChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, tex->data);
		glGenerateMipmap(GL_TEXTURE_2D);
		m_isBase = tex->isBaseAsset;
		tex->alreadyLoaded = true;
		tex->loadedTexture = this;
	}

	void Texture::bind(unsigned int slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glActiveTexture(GL_TEXTURE0);
	}

	Vec2 Texture::getDimensions() {
		return { (float)m_x, (float)m_y };
	}

	bool Texture::isBaseAsset() {
		return m_isBase;
	}

	Texture::~Texture() {
		glDeleteTextures(1, &m_id);
	}
}
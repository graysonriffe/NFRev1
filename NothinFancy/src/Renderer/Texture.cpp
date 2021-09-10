#include "Texture.h"

#include "GL/glew.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Assets.h"
#include "Utility.h"

namespace nf {
	Texture::Texture(ATexture* tex) :
		m_isBase(false),
		m_id(0),
		m_x(0),
		m_y(0)
	{
		int nChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* texture = stbi_load_from_memory((unsigned char*)tex->data, tex->size, &m_x, &m_y, &nChannels, 0);
		if (!texture)
			Error("Texture failed to load from memory!");
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_x, m_y, 0, nChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, texture);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(texture);
		m_isBase = tex->isBaseAsset;
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
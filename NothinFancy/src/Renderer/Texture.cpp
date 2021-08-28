#include "Texture.h"

#include "GL/glew.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Utility.h"

namespace nf {
	Texture::Texture() {
		glGenTextures(1, &m_id);
	}

	void Texture::create(const char* textureName) {
		int width, height, nChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* texture = stbi_load(textureName, &width, &height, &nChannels, 0);
		//TODO: Load from memory
		if (!texture)
			Error("Texture \"" + (std::string)textureName + "\" failed to load from memory!");
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
		//glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(texture);
	}

	void Texture::bind() {
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	Texture::~Texture() {
		glDeleteTextures(1, &m_id);
	}
}
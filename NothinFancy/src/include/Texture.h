#pragma once

namespace nf {
	class Texture {
	public:
		Texture();

		void create(const unsigned char* textureData, size_t textureSize);
		void bind();

		~Texture();
	private:
		unsigned int m_id;
	};
}
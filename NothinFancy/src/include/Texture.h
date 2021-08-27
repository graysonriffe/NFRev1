#pragma once

namespace nf {
	class Texture {
	public:
		Texture();

		void create(const char* textureName);
		void bind();

		~Texture();
	private:
		unsigned int m_id;
	};
}
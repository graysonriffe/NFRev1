#pragma once
#include "Utility.h"

namespace nf {
	class Texture {
	public:
		Texture();

		void create(const char* textureData, size_t textureSize);
		void bind();
		Vec2 getDimensions();

		~Texture();
	private:
		unsigned int m_id;
		int m_x;
		int m_y;
	};
}
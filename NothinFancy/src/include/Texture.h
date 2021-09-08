#pragma once
#include "Utility.h"

namespace nf {
	struct ATexture;

	class Texture {
	public:
		Texture();

		void create(ATexture* tex);
		void bind();
		Vec2 getDimensions();
		bool isBaseAsset();

		~Texture();
	private:
		bool m_isBase;
		unsigned int m_id;
		int m_x;
		int m_y;
	};
}
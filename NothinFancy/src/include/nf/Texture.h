#pragma once
#include "nf/Utility.h"

namespace nf {
	struct ATexture;

	class Texture {
	public:
		Texture(ATexture* tex, bool linear = false);

		void bind(unsigned int slot = 0);
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
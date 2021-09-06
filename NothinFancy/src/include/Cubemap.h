#pragma once
#include "Drawable.h"

namespace nf {
	struct Asset;

	class Cubemap : public Drawable {
	public:
		Cubemap();

		void create(Asset* cubemapAsset);
		bool isConstructed();
		void render();

		~Cubemap();
	private:
		bool m_constructed;
		unsigned int m_id;
	};
}
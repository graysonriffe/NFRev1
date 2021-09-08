#pragma once
#include "NFObject.h"
#include "Drawable.h"

namespace nf {
	struct Asset;

	class Cubemap : public Drawable, public NFObject {
	public:
		Cubemap();

		void create(Asset* cubemapAsset);
		bool isConstructed();
		void render();

		void destroy() override;
		~Cubemap();
	private:
		bool m_constructed;
		unsigned int m_id;
	};
}
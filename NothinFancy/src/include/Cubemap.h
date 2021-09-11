#pragma once
#include "NFObject.h"
#include "Drawable.h"

namespace nf {
	struct Asset;
	class Shader;

	class Cubemap : public Drawable, public NFObject {
	public:
		Cubemap();

		void create(Asset* cubemapAsset);
		bool isConstructed();
		void render(Shader* shader);

		void destroy() override;
		~Cubemap();
	private:
		bool m_constructed;
		unsigned int m_id;
	};
}
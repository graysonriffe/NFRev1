#pragma once
#include "UIElement.h"
#include "NFObject.h"
#include "Utility.h"

namespace nf {
	class Texture;
	struct Asset;

	class UITexture : public UIElement, public NFObject {
	public:
		UITexture();

		void create(Asset* textureAsset, const Vec2& position, float scale = 1.0f, float opacity = 1.0f);
		const char* identity() override;
		void setScale(float scale);
		void setOpacity(float opacity);
		void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) override;

		void destroy() override;
		~UITexture();
	private:
		Texture* m_texture;
		float m_scale;
		float m_opacity;
	};
}
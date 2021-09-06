#pragma once
#include "UIElement.h"
#include "Utility.h"

namespace nf {
	class Texture;
	struct Asset;

	class UITexture : public UIElement {
	public:
		UITexture();

		void create(Asset* textureAsset, const Vec2& position, double scale = 1.0, double opacity = 1.0 );
		const char* identity() override;
		void setScale(double scale);
		void setOpacity(double opacity);
		void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) override;

		~UITexture();
	private:
		Texture* m_texture;
		float m_scale;
		float m_opacity;
	};
}
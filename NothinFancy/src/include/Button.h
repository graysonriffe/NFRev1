#pragma once
#include <string>

#include "UIElement.h"
#include "NFObject.h"
#include "Text.h"

namespace nf {
	class Application;
	class Texture;
	struct Asset;

	class Button : public UIElement, public NFObject {
	public:
		Button();

		void create(const Vec2& position, std::string string = "", Asset* buttonAsset = nf::BaseAssets::button, double scale = 1.0, double opacity = 1.0);
		const char* identity() override;
		void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight, Application* app, Shader* textShader);
		bool isClicked();

		void destroy() override;
		~Button();
	private:
		Texture* m_idleTexture;
		Texture* m_hoverTexture;
		Texture* m_pressedTexture;
		Texture* m_currentTexture;
		std::string m_string;
		Text m_text;
		float m_scale;
		float m_opacity;
		bool m_clicked;
		bool m_triggered;
	};
}
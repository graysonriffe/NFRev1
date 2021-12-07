#pragma once
#include <string>

#include "nf/UIElement.h"
#include "nf/NFObject.h"
#include "nf/Text.h"

namespace nf {
	class Application;
	class Texture;
	struct Asset;

	/**
	 * @brief A UI button
	 * 
	 * Buttons can be customized to use three different textures: an idle, hover, and pressed
	 * texture.
	 * 
	 * @sa @ref customButtons
	 * @ref createUITut
	*/
	class Button : public UIElement, public NFObject {
	public:
		Button(const Button& other) = delete;
		/**
		 * @brief Constructor
		 * 
		 * See @ref obLifetime
		*/
		Button();
		/**
		 * @brief Creates a UI button
		 * @param position Position vector in screen space
		 * @param string Text to show on the button; Can be an empty string; Optional
		 * @param buttonAsset A custom button asset; Optional
		 * @param scale Scale of the button; Optional
		 * @param opacity Opacity of the button; Optional
		*/
		void create(const Vec2& position, std::string string = "", Asset* buttonAsset = nf::BaseAssets::button, float scale = 1.0f, float opacity = 1.0f);
		/**
		 * @brief Queries whether or not the button has been clicked
		 * @return If the button has been clicked
		 * 
		 * @note This function returns true for only one frame when the mouse button
		 * is released on top of the button.
		*/
		bool isClicked();
#ifndef NFIMPL
		const char* identity() override;
		void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight, Application* app, Shader* textShader);
#endif
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
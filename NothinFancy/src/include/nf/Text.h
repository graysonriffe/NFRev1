#pragma once
#include <map>

#include "nf/UIElement.h"
#include "nf/NFObject.h"
#include "nf/Assets.h"
#include "nf/Utility.h"

namespace nf {
	struct Character {
		unsigned int texID = 0;
		Vec2 size;
		Vec2 bearing;
		unsigned int advance = 0;
	};

	struct Font {
		std::map<char, Character> m_characters;
		bool isBase = false;
	};

	/**
	 * @brief A UI string
	 * 
	 * Custom fonts are supported. See @ref customFonts
	*/
	class Text : public UIElement, public NFObject {
	public:
		/**
		 * @brief Constructor
		 * 
		 * See @ref obLifetime
		*/
		Text();
		/**
		 * @brief Creates UI text
		 * @param string The text itself
		 * @param position Position vector in screen space
		 * @param color Color vector; Optional
		 * @param scale Scale of the text; Optional
		 * @param opacity Opacity of the text; Optional
		 * @param fontAsset Custom font asset; Optional
		*/
		void create(const std::string& string, const Vec2& position, const Vec3& color = {1.0, 1.0, 1.0}, float scale = 1.0f, float opacity = 1.0f, Asset* fontAsset = BaseAssets::font);
		/**
		 * @brief Sets the displayed text
		 * @param string The new text to display
		*/
		void setText(const std::string& string);
		/**
		 * @brief Sets the text's color
		 * @param color The new color vector
		*/
		void setColor(const Vec3& color);
		/**
		 * @brief Sets the text's scale
		 * @param scale The new scale
		*/
		void setScale(float scale);
		/**
		 * @brief Sets the text's opacity
		 * @param opacity The new opacity
		*/
		void setOpacity(float opacity);
#ifndef NFIMPL
		const char* identity() override;
		void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight, bool onButton = false, float buttonWidth = 0.0f, float buttonHeight = 0.0f, const Vec2& buttonPos = Vec2());
#endif
		void destroy() override;
		~Text();
	private:
		std::string m_string;
		Font* m_font;
		Vec3 m_color;
		float m_scale;
		float m_opacity;
	};
}
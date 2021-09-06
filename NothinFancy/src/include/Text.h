#pragma once
#include <map>

#include "UIElement.h"
#include "Assets.h"
#include "Utility.h"

namespace nf {
	struct Character {
		unsigned int texID = 0;
		Vec2 size;
		Vec2 bearing;
		unsigned int advance = 0;
	};

	struct Font {
		std::map<char, Character> m_characters;
	};

	class Text : public UIElement {
	public:
		Text();

		void create(const std::string& string, const Vec2& position, const Vec3& color = {1.0, 1.0, 1.0}, double opacity = 1.0, double scale = 1.0, Asset* font = BaseAssets::defaultFont);
		const char* identity() override;
		void setText(const std::string& string);
		void setColor(const Vec3& color);
		void setScale(double scale);
		void setOpacity(double opacity);
		void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) override;

		~Text();
	private:
		std::string m_string;
		Font* m_font;
		Vec3 m_color;
		float m_scale;
		float m_opacity;
	};
}
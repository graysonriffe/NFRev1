#pragma once
#include <map>

#include "UIElement.h"
#include "Assets.h"
#include "Utility.h"

namespace nf {
	struct Character {
		unsigned int texID;
		Vec2 size;
		Vec2 bearing;
		unsigned int advance;
	};

	struct Font {
		std::map<char, Character> m_characters;
	};

	class Text : public UIElement {
	public:
		Text();

		void create(const std::string& string, const Vec2& position, const Vec3& color = {1.0, 1.0, 1.0}, unsigned int size = 160, Asset* font = BaseAssets::defaultFont);
		void setText(const std::string& string);
		const char* identity() override;
		void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) override;

		~Text();
	private:
		std::string m_string;
		Font* m_font;
		Vec3 m_color;
	};
}
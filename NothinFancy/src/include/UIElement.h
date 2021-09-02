#pragma once
#include "Drawable.h"

#include "Utility.h"

namespace nf {
	class Shader;

	class UIElement : public Drawable {
	public:
		UIElement();

		virtual const char* identity();
		void setPosition(float x, float y);
		void setPosition(const Vec2& position);
		void centered(bool x, bool y = false);
		virtual void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight);

		~UIElement();
	protected:
		Vec2 m_position;
		bool m_centeredX;
		bool m_centeredY;
	};
}
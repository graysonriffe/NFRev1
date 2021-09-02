#pragma once
#include "Drawable.h"

namespace nf {
	class Shader;

	class UIElement : public Drawable {
	public:
		UIElement();

		virtual const char* identity();
		virtual void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight);

		~UIElement();
	private:

	};
}
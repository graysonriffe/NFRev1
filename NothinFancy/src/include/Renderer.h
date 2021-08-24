#pragma once
#include <vector>
#include <Windows.h>

#include "Drawable.h"

namespace nf {
	class Application;

	class Renderer {
	public:
		Renderer(Application* app);

		void render(const Drawable& in);

		void doFrame();

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		std::vector<Drawable*> m_lGame;
		std::vector<Drawable*> m_lUI;
	};
}
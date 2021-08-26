#pragma once
#include <vector>
#include <Windows.h>

#include "Model.h"

namespace nf {
	class Application;

	class Renderer {
	public:
		Renderer(Application* app);

		void render(Drawable& in);

		void doFrame();

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		std::vector<Drawable*> m_lGame;
		std::vector<Drawable*> m_lUI;
		const char* m_defaultVertex;
		const char* m_defaultFragment;
		Shader* m_defaultShader;
	};
}
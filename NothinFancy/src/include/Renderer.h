#pragma once
#include <Windows.h>

namespace nf {
	class Application;

	class Renderer {
	public:
		Renderer(Application* app);

		void doFrame();

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;
	};
}
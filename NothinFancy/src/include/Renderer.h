#pragma once
#include <vector>
#include <Windows.h>

#include "Entity.h"
#include "Camera.h"

namespace nf {
	class Application;

	class Renderer {
	public:
		Renderer(Application* app);

		void render(Entity& in);
		//TODO: Create second render function for UIElements

		void doFrame(Camera* camera);

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		std::vector<Entity*> m_lGame;
		std::vector<Drawable*> m_lUI;
		Shader* m_defaultShader;
	};
}
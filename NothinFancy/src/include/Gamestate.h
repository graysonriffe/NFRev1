#pragma once
#include "Camera.h"

namespace nf {
	class Application;
	class Renderer;

	class Gamestate {
	public:
		Gamestate(Application* app);
		Gamestate() = delete;
		Gamestate(const Gamestate& other) = delete;

		virtual void onEnter();

		virtual void update(double deltaTime);
		Camera* getCamera();
		virtual void render(Renderer& renderer);

		virtual void onExit();
	protected:
		Application* m_app;
		Camera m_camera;
	};
}
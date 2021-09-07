#pragma once
#include "Camera.h"

namespace nf {
	class Application;
	class Renderer;
	class Camera;
	class Gamestate {
	public:
		Gamestate();

		Gamestate(const Gamestate& other) = delete;

		void setup(Application* app);

		virtual void onEnter();

		virtual void update(double deltaTime);
		Camera* getCamera();
		virtual void render(Renderer& renderer);

		virtual void onExit();
	protected:
		Application* app;
		Camera* camera;
	};
}
#pragma once
#include <vector>

#include "Camera.h"
#include "NFObject.h"

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
		void cleanup();

		std::vector<NFObject*> m_nfObjects;
	protected:
		Application* app;
		Camera* camera;
	};
}
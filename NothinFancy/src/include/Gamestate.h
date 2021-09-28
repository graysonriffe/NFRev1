#pragma once
#include <vector>
#include <unordered_set>

#include "Camera.h"
#include "NFObject.h"

namespace nf {
	class Application;
	class Renderer;
	class Camera;
	class Model;

	class Gamestate {
	public:
		Gamestate();

		Gamestate(const Gamestate& other) = delete;

		void setup(Application* app);

		virtual void onEnter();
		bool isRunning();
		void setRunning();

		virtual void update(double deltaTime);
		Camera* getCamera();
		virtual void render(Renderer& renderer);

		virtual void onExit();
		void cleanup();

		std::vector<NFObject*> m_nfObjects;
		std::unordered_set<Model*> m_modelsToDelete;
	protected:
		Application* app;
		Camera* camera;
		bool m_running;
	};
}
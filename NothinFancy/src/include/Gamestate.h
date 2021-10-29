#pragma once
#include <vector>
#include <unordered_set>

#include "Camera.h"
#include "NFObject.h"

namespace nf {
	class Application;
	class Renderer;
	class Camera;
	class Entity;
	class Model;
	class Texture;

	class Gamestate {
	public:
		Gamestate();

		Gamestate(const Gamestate& other) = delete;

		virtual void onEnter();
		bool isRunning();
		bool isLoading();
		void run(Application* app, bool physics = true);

		virtual void update(float deltaTime);
		Camera* getCamera();
		virtual void render(Renderer& renderer);

		virtual void onExit();
		void stop();

		std::vector<NFObject*> m_nfObjects;
		std::vector<Entity*> m_entsToDelete;
		std::unordered_set<Model*> m_modelsToDelete;
		std::unordered_set<Texture*> m_texturesToDelete;
	protected:
		Application* app;
		Camera* camera;
		bool m_loading;
		bool m_running;
	};
}
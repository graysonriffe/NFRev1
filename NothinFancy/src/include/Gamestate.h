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
		//TODO: Add this to other objects
		Gamestate(const Gamestate& other) = delete;

		void run(Application* app, bool physics = true);
		bool isRunning();
		bool isLoading();

		virtual void onEnter();

		virtual void update(float deltaTime);
		virtual void render(Renderer& renderer);

		Camera* getCamera();
		//In units of Earth gravity (9.81 m/s^2)
		void setGravity(const Vec3& gravity);
		void setGravity(float strength);

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
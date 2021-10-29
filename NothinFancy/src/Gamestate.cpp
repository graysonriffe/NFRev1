#include "Gamestate.h"

#include "Application.h"
#include "PhysicsEngine.h"
#include "Entity.h"
#include "Model.h"
#include "Texture.h"
#include "Utility.h"

namespace nf {
	Gamestate::Gamestate() :
		app(nullptr),
		camera(nullptr),
		m_loading(false),
		m_running(false)
	{

	}

	void Gamestate::run(Application* app, bool physics) {
		m_loading = true;
		this->app = app;
		camera = new Camera(this->app);

		if (physics)
			app->getPhysicsEngine()->newScene();
		onEnter();

		m_loading = false;
		m_running = true;
	}

	void Gamestate::onEnter() {

	}

	bool Gamestate::isRunning() {
		return m_running;
	}

	bool Gamestate::isLoading() {
		return m_loading;
	}

	void Gamestate::update(float deltaTime) {

	}

	Camera* Gamestate::getCamera() {
		return camera;
	}

	void Gamestate::render(Renderer& renderer) {

	}

	void Gamestate::onExit() {

	}

	void Gamestate::stop() {
		onExit();
		app->getPhysicsEngine()->closeScene();

		for (NFObject* curr : m_nfObjects)
			curr->destroy();
		m_nfObjects.clear();

		for (Entity* curr : m_entsToDelete)
			delete curr;
		m_entsToDelete.clear();

		for (Model* curr : m_modelsToDelete)
			if (!curr->isBaseAsset())
				delete curr;
		m_modelsToDelete.clear();

		for (Texture* curr : m_texturesToDelete)
			if (!curr->isBaseAsset())
				delete curr;
		m_texturesToDelete.clear();

		delete camera;
		app = nullptr;
		m_running = false;
	}
}
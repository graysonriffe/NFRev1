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

	bool Gamestate::isRunning() {
		return m_running;
	}

	bool Gamestate::isLoading() {
		return m_loading;
	}

	void Gamestate::onEnter() {
		Error("Gamestate has no overridden onEnter function!");
	}

	void Gamestate::update(float deltaTime) {
		Error("Gamestate has no overridden update function!");
	}

	void Gamestate::render(Renderer& renderer) {
		Error("Gamestate has no overridden render function!");
	}

	Camera* Gamestate::getCamera() {
		return camera;
	}

	void Gamestate::setAmbientLight(float strength) {
		app->getRenderer()->setAmbient(strength);
	}

	void Gamestate::setGravity(const Vec3& gravity) {
		app->getPhysicsEngine()->setGravity(gravity * 9.81f);
	}

	void Gamestate::setGravity(float strength) {
		app->getPhysicsEngine()->setGravity(Vec3(0.0f, -9.81f * strength, 0.0f));
	}

	void Gamestate::onExit() {
		Error("Gamestate has no overridden onExit function!");
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
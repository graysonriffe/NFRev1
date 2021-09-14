#include "Gamestate.h"

#include "Application.h"
#include "Utility.h"

namespace nf {
	Gamestate::Gamestate() :
		app(nullptr),
		camera(nullptr),
		m_running(false)
	{

	}

	void Gamestate::setup(Application* app) {
		this->app = app;
		camera = new Camera(this->app);
		m_running = false;
	}

	void Gamestate::onEnter() {

	}

	bool Gamestate::isRunning() {
		return m_running;
	}

	void Gamestate::setRunning() {
		m_running = true;
	}

	void Gamestate::update(double deltaTime) {

	}

	Camera* Gamestate::getCamera() {
		return camera;
	}

	void Gamestate::render(Renderer& renderer) {

	}

	void Gamestate::onExit() {

	}

	void Gamestate::cleanup() {
		for (NFObject* curr : m_nfObjects)
			curr->destroy();
		m_nfObjects.clear();

		delete camera;
		app = nullptr;
	}
}
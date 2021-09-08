#include "Gamestate.h"

#include "Application.h"
#include "Utility.h"

namespace nf {
	Gamestate::Gamestate() :
		app(nullptr),
		camera(nullptr)
	{

	}

	void Gamestate::setup(Application* app) {
		this->app = app;
		camera = new Camera(this->app);
	}

	void Gamestate::onEnter() {

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
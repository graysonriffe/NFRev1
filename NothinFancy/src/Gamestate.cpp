#include "Gamestate.h"

#include "Application.h"
#include "Utility.h"

namespace nf {
	Gamestate::Gamestate(Application* app) :
		camera(app)
	{
		this->app = app;
	}

	void Gamestate::onEnter() {

	}

	void Gamestate::update(double deltaTime) {

	}

	Camera* Gamestate::getCamera() {
		return &camera;
	}

	void Gamestate::render(Renderer& renderer) {

	}

	void Gamestate::onExit() {

	}
}
#include "Gamestate.h"

#include "Application.h"
#include "Utility.h"

namespace nf {
	Gamestate::Gamestate(Application* app) {
		m_app = app;
		m_renderer = m_app->getRenderer();
	}

	void Gamestate::onEnter() {

	}

	void Gamestate::update(double deltaTime) {

	}

	void Gamestate::render() {

	}

	void Gamestate::onExit() {

	}
}
#include "MainState.h"

MainState::MainState(nf::Application* app) :
	Gamestate(app)
{
}

void MainState::onEnter() {
	Log("MainState onEnter!");
}

void MainState::update(double deltaTime) {
}

void MainState::render() {
	static nf::Renderer& renderer = *m_app->getRenderer();
}

void MainState::onExit() {
	Log("MainState onExit!");
}
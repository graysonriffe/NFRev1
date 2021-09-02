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

void MainState::render(nf::Renderer& renderer) {

}

void MainState::onExit() {
	Log("MainState onExit!");
}
#include "MainState.h"

MainState::MainState(Application* app) :
	Gamestate(app)
{
}

void MainState::onEnter() {
	Log("MainState onEnter!");
}

void MainState::update(double deltaTime) {

}

void MainState::render() {

}

void MainState::onExit() {
	Log("MainState onExit!");
}
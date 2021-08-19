#include "MainState.h"

void MainState::onEnter(Application* app) {
	Log("MainState onEnter!");
	m_app = app;
}

void MainState::onExit() {
	Log("MainState onExit!");
}

void MainState::update() {
	Log("MainState update!");
}

void MainState::render() {
	Log("MainState render!");
}
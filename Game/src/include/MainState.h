#pragma once
#include "NothinFancy.h"

class MainState : public nf::Gamestate {
public:
	MainState(nf::Application* app);

	void onEnter() override;

	void update(double deltaTime) override;
	void render() override;

	void onExit() override;
private:

};
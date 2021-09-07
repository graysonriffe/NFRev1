#pragma once
#include "NothinFancy.h"

class MainState : public nf::Gamestate {
public:
	void onEnter() override;

	void update(double deltaTime) override;
	void render(nf::Renderer& renderer) override;

	void onExit() override;
private:

};
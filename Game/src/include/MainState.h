#pragma once
#include "NothinFancy.h"

class MainState : public nf::IGamestate {
public:
	void onEnter(Application* app) override;
	void onExit() override;

	void update() override;
	void render() override;
private:

};
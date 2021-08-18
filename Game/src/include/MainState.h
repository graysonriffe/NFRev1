#pragma once
#include "NothinFancy.h"

class MainState : public nf::IGamestate {
public:
	void onEnter() override;
	void onExit() override;

	void update() override;
	void render() override;
private:

};
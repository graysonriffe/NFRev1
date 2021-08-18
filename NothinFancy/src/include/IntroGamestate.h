#pragma once
#include "IGamestate.h"
#include "Utility.h"

namespace nf {
	class IntroGamestate : public IGamestate {
	public:
		void onEnter(Application* app) override;
		void onExit() override;

		void update() override;
		void render() override;
	private:
		int counter;
	};
}
#pragma once
#include "Gamestate.h"

namespace nf {
	class IntroGamestate : public Gamestate {
	public:
		IntroGamestate(Application* app);

		void onEnter() override;

		void update(double deltaTime) override;
		void render() override;

		void onExit() override;
	private:
		int m_counter;
		//TODO: Flesh out intro gamestate
	};
}
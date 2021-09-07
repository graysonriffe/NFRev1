#include "IntroGamestate.h"

#include "Application.h"
#include "Utility.h"
#include "Input.h"

namespace nf {
	void IntroGamestate::onEnter() {
		Log("Intro onEnter!");
		logoTex.create(BaseAssets::logo, Vec2(0.0, 0.0));
		logoTex.centered(true, true);
		text.create("(C) Grayson Riffe 2021", Vec2(0.01, 0.025), Vec3(0.8));
		text.setScale(0.6);
		m_counter = 0;
	}

	void IntroGamestate::update(double deltaTime) {
		if (m_counter >= 240) {
			app->changeState(app->getDefaultState());
		}

		static double scale = 2.0;
		logoTex.setScale(scale);
		if (m_counter >= 20) {
			scale += 0.002;
		}
		m_counter++;

		if (app->isInput(NFI_SPACE))
			m_counter = 240;
		if (app->isInput(NFI_ESCAPE))
			app->quit();
	}

	void IntroGamestate::render(Renderer& renderer) {
		renderer.render(logoTex);
		renderer.render(text);
	}

	void IntroGamestate::onExit() {
		Log("Intro onExit!");
	}
}
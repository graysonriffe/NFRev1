#include "IntroGamestate.h"

#include "Application.h"
#include "Utility.h"
#include "Input.h"

namespace nf {
	void IntroGamestate::onEnter() {
		Log("Intro onEnter!");
		m_logoTex.create(BaseAssets::logo, Vec2(0.0, 0.0));
		m_logoTex.centered(true, true);
		m_text.create("(C) Grayson Riffe 2021", Vec2(0.01, 0.025), Vec3(0.8));
		m_text.setScale(0.6);
		m_start = std::chrono::steady_clock::now();
	}

	void IntroGamestate::update(double deltaTime) {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::ratio<1i64>> dur = now - m_start;

		static double scale = 2.0;
		m_logoTex.setScale(scale);
		if (dur.count() > 0.2) {
			scale += 0.12 * deltaTime;
		}

		if (dur.count() > 3.5 || app->isInput(NFI_SPACE)) {
			app->changeState(app->getDefaultState());
		}
		if (app->isInput(NFI_ESCAPE))
			app->quit();
	}

	void IntroGamestate::render(Renderer& renderer) {
		renderer.render(m_logoTex);
		renderer.render(m_text);
	}

	void IntroGamestate::onExit() {
		Log("Intro onExit!");
	}
}
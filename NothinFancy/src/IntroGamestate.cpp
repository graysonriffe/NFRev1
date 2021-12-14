#include "nf/IntroGamestate.h"

#include "nf/Application.h"
#include "nf/Utility.h"
#include "nf/Input.h"

namespace nf {
	void IntroGamestate::onEnter() {
		m_scale = 2.0;
		m_logoTex.create(BaseAssets::logo, Vec2(0.0, 0.0));
		m_logoTex.centered(true, true);
		m_verText.create("v" + (std::string)NFVERSION, Vec2(0.01f, 0.075f), Vec3(0.8f), 0.6f);
		m_text.create("(c) Grayson Riffe 2021 | graysonriffe.com", Vec2(0.01f, 0.025f), Vec3(0.8f), 0.6f);
		m_start = std::chrono::steady_clock::now();
	}

	void IntroGamestate::update(float deltaTime) {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		std::chrono::duration<float, std::ratio<1i64>> dur = now - m_start;

		m_logoTex.setScale(m_scale);
		if (dur.count() > 0.2) {
			m_scale += 0.12f * deltaTime;
		}

		if (dur.count() > 3.5 || app->isKeyPressed(NFI_SPACE)) {
			app->changeState(app->getDefaultState());
		}
		if (app->isKeyPressed(NFI_ESCAPE))
			app->quit();
	}

	void IntroGamestate::render(Renderer& renderer) {
		renderer.render(m_logoTex);
		renderer.render(m_verText);
		renderer.render(m_text);
	}

	void IntroGamestate::onExit() {

	}
}
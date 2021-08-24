#include "IntroGamestate.h"

#include "Application.h"
#include "Utility.h"

namespace nf {
	void IntroGamestate::onEnter(Application* app) {
		Log("Intro onEnter!");
		m_app = app;
		counter = 0;
	}

	void IntroGamestate::onExit() {
		Log("Intro onExit!");
	}

	void IntroGamestate::update() {
		Log("Intro update!");
		if (counter >= 120) {
			m_app->changeState("Main State");
		}
		counter++;
	}

	void IntroGamestate::render() {
		Log("Intro render!");
	}
}
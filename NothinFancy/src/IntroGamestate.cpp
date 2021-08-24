#include "IntroGamestate.h"

#include "Application.h"
#include "Utility.h"

namespace nf {
	IntroGamestate::IntroGamestate(Application* app) :
		Gamestate(app),
		m_counter(0)
	{
	}

	void IntroGamestate::onEnter() {
		Log("Intro onEnter!");
		m_counter = 0;
	}


	void IntroGamestate::update(double deltaTime) {
		if (m_counter >= 120) {
			m_app->changeState("Main State");
		}
		m_counter++;
	}

	void IntroGamestate::render() {
	}

	void IntroGamestate::onExit() {
		Log("Intro onExit!");
	}
}
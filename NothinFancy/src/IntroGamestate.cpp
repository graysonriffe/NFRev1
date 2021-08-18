#include "IntroGamestate.h"

namespace nf {
	void IntroGamestate::onEnter() {
		Log("Intro onEnter!");
	}

	void IntroGamestate::onExit() {
		Log("Intro onExit!");
	}

	void IntroGamestate::update() {
		Log("Intro update!");
	}

	void IntroGamestate::render() {
		Log("Intro render!");
	}
}
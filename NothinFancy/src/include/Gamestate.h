#pragma once

namespace nf {
	class Gamestate {
	public:
		Gamestate();

		virtual void onEnter();
		virtual void onExit();
	private:
		//Resource stuff
	};
}
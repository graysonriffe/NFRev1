#pragma once

namespace nf {
	class IGamestate {
	public:
		virtual void onEnter() = 0;
		virtual void onExit() = 0;

		virtual void update() = 0;
		virtual void render() = 0;
	private:
		//Resource identifier?
	};
}
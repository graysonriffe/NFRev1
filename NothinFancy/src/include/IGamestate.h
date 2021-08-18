#pragma once

namespace nf {
	class Application;

	class IGamestate {
	public:
		virtual void onEnter(Application* app) = 0;
		virtual void onExit() = 0;

		virtual void update() = 0;
		virtual void render() = 0;

		Application* m_app;
	private:
		//Resource identifier?
	};
}
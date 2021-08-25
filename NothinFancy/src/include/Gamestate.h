#pragma once

namespace nf {
	class Application;
	class Renderer;

	class Gamestate {
	public:
		Gamestate(Application* app);
		Gamestate() = delete;
		Gamestate(const Gamestate& other) = delete;

		virtual void onEnter();

		virtual void update(double deltaTime);
		virtual void render();

		virtual void onExit();
	protected:
		Application* m_app;
		//Resource identifier?
	};
}
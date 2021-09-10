#pragma once
#include <chrono>
#include <unordered_map>
#include <array>
#include <Windows.h>

#include "Config.h"
#include "IntroGamestate.h"
#include "Renderer.h"
//TODO: Document ALL frontend functions

namespace nf {
	class Application {
	public:
		Application(Config& conf);
		Application() = delete;
		Application(Application& other) = delete;

		void setWindowIcon(HANDLE hIcon);
		void setWindowCursor(HCURSOR hCursor);
		Renderer* getRenderer() const;
		void addState(Gamestate* state, const std::string& stateName);
		void addDefaultState(const std::string& stateName);
		const std::string& getDefaultState();
		void run();
		void changeState(const std::string& stateName);
		Gamestate* getCurrentState();
		void showWindow(bool show);
		const HWND& getWindow();
		void changeConfig(const Config& in);
		const Config& getConfig() const;
		int getFPS() const;
		bool isKeyHeld(unsigned int code);
		bool isKeyPressed(unsigned int code);
		void trackMouse(bool track);
		void getMouseDiff(int& x, int& y);
		static Application* getApp();

		void quit();
		~Application();
	private:
		void registerWindowClass();
		RECT getWindowRect() const;
		void calculateNewWindowPos(int& x, int& y);
		void toggleFullscreen();
		void updateMouse();
		void runMainGameThread();
		void doStateChange();
		static void setApp(Application* app);

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		static Application* currentApp;
		Config m_currentConfig;
		bool m_running;
		bool m_quit;
		HINSTANCE m_hInst;
		LPCWSTR m_wclassName;
		HWND m_window;
		LONG m_defaultWindowStyle;
		unsigned int m_altWidth, m_altHeight;

		std::chrono::duration<double> m_fpsDuration;
		double m_deltaTime;
		std::chrono::steady_clock::time_point m_fpsClock1 = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point m_fpsClock2 = m_fpsClock1;
		const int m_targetFPS = 60;
		const double m_minFrametime = 1.0 / m_targetFPS;
		int m_FPS;

		//Inactive states to potentially be active during the Application's lifetime
		//Mapped to const char* to be referenced later in the frontend
		std::unordered_map<std::string, Gamestate*> m_states;
		Gamestate* m_sIntro;
		std::string m_defaultState;
		bool m_defaultStateAdded;
		Gamestate* m_currentState;
		bool m_stateChange;
		std::string m_nextState;

		//Array of booleans that represent keyboard and mouse input minus the scrollwheel
		std::array<std::pair<bool, bool>, 164> m_keys;
		unsigned int m_mouseX, m_mouseY;
		bool m_trackingMouse;
		bool m_mouseTrackFirst;
		int m_mouseDiffX, m_mouseDiffY;

		//Renderer object to use OpenGL to render the current state
		Renderer* m_renderer;
	};
}
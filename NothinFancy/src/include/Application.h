#pragma once
#include "Config.h"
#include "Utility.h"
#include "IntroGamestate.h"
#include <Windows.h>
#include <chrono>
#include <unordered_map>
#include <vector>

namespace nf {
	class Application {
	public:
		Application(Config& conf);
		Application() = delete;
		Application(Application& other) = delete;

		void setWindowIcon(HANDLE hIcon);
		void setWindowCursor(HCURSOR hCursor);
		void addState(const char* stateName, IGamestate* state);
		void addDefaultState(const char* stateName);
		void changeState(const char* stateName);
		void run();
		void showWindow(bool show);
		const Config& getConfig() const;
		int getFPS() const;

		~Application();
	private:
		void addIntroState();
		void registerWindowClass();
		void toggleFullscreen();
		RECT getWindowRect() const;

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void createOpenGLContext();

		Config m_currentConfig;
		bool m_running;
		HINSTANCE m_hInst;
		LPCWSTR m_wclassName;
		HWND m_window;
		LONG m_defaultWindowStyle;
		WINDOWPLACEMENT m_wndPlacement;
		HDC m_hdc;
		HGLRC m_hglrc;

		std::chrono::steady_clock::time_point m_frameClock = std::chrono::steady_clock::now();
		std::chrono::duration<double> m_fpsDuration;
		double m_deltaTime;
		std::chrono::steady_clock::time_point m_fpsClock1 = m_frameClock;
		std::chrono::steady_clock::time_point m_fpsClock2 = m_frameClock;
		int m_frames;
		const int m_targetFPS = 60;
		const double m_minFrametime = 1.0 / m_targetFPS;
		int m_FPS;

		//Inactive states states to potentially be loaded during the Application's lifetime
		std::unordered_map<const char*, IGamestate*> m_states;
		//The currently active and loaded states where the top-most is the current one
		std::vector<IGamestate*> m_activeStates;
		IntroGamestate* m_sIntro;
		IGamestate* m_DefaultState;
		bool m_defaultStateAdded = false;
		IGamestate* m_currentState;
	};
}
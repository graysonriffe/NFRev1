#pragma once
#include <chrono>
#include <unordered_map>
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
		void addState(IGamestate* state, const std::string& stateName);
		void addDefaultState(const std::string& stateName);
		void run();
		void changeState(const std::string& stateName);
		void showWindow(bool show);
		const HWND& getWindow();
		void changeConfig(const Config& in);
		const Config& getConfig() const;
		int getFPS() const;
		bool isInput(unsigned int code);

		~Application();
	private:
		void registerWindowClass();
		RECT getWindowRect() const;
		void calculateNewWindowPos(int& x, int& y);
		void toggleFullscreen();
		void runMainGameThread();
		void startIntroState();
		void doStateChange();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		Config m_currentConfig;
		bool m_running;
		HINSTANCE m_hInst;
		LPCWSTR m_wclassName;
		HWND m_window;
		LONG m_defaultWindowStyle;
		WINDOWPLACEMENT m_wndPlacement;

		std::chrono::duration<double> m_fpsDuration;
		double m_deltaTime;
		std::chrono::steady_clock::time_point m_fpsClock1 = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point m_fpsClock2 = m_fpsClock1;
		int m_frames;
		const int m_targetFPS = 60;
		const double m_minFrametime = 1.0 / m_targetFPS;
		int m_FPS;

		//Inactive states to potentially be active during the Application's lifetime
		//Mapped to const char* to be referenced later in the frontend
		std::unordered_map<std::string, IGamestate*> m_states;
		IntroGamestate* m_sIntro;
		IGamestate* m_DefaultState;
		bool m_defaultStateAdded;
		IGamestate* m_currentState;
		bool m_stateChange;
		std::string m_nextState;

		//Array of booleans that represent keyboard and mouse input minus the scrollwheel
		bool m_input[164];

		//Renderer object to use OpenGL to render the current state
		Renderer* m_renderer;
	};
}
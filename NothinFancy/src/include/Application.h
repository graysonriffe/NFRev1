#pragma once
#include "Config.h"
#include <Windows.h>
#include <chrono>
#include <vector>

namespace nf {
	class Application {
	public:
		Application(Config& conf);
		Application() = delete;
		Application(Application& other) = delete;

		void setWindowIcon(HANDLE hIcon);
		void setWindowCursor(HCURSOR hCursor);
		void startLoop();
		void showWindow(bool show);
		Config& getConfig();
		int getFPS();

		~Application();
	private:
		void registerWindowClass();
		void toggleFullscreen();
		RECT getWindowRect();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void createOpenGLContext();

		Config m_currentConfig;
		bool m_running;
		HINSTANCE m_hInst;
		LPCWSTR m_wclassName;
		HWND m_window;
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

		
	};
}
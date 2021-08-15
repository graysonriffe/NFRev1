#pragma once
#include "Config.h"
#include <Windows.h>

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
	};
}
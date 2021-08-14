#pragma once
#include "Config.h"
#include <Windows.h>

namespace nf {
	class Application {
	public:
		Application(Config& conf);
		Application() = delete;
		Application(Application& other) = delete;

		Config& getConfig();
		void showWindow(bool show);

		~Application();
	private:
		void registerWindowClass();
		void toggleFullscreen();
		RECT getWindowRect();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		Config m_currentConfig;
		HINSTANCE m_hInst;
		LPCWSTR m_wclassName;
		HWND m_window;
		WINDOWPLACEMENT m_wndPlacement;
	};
}
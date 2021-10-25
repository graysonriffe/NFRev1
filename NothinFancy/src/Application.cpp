#include "Application.h"

#include <thread>

#include "Utility.h"

namespace nf {
	DEBUGINIT;

	Application::Application(Config& config) :
		m_currentConfig(config),
		m_running(false),
		m_quit(false),
		m_customWindowIconSet(false),
		m_altWidth(1280),
		m_altHeight(720),
		m_defaultStateAdded(false),
		m_stateChange(false)
	{
		Log("Creating NF application");
		Log("Width: " + std::to_string(m_currentConfig.width) + ", Height: " + std::to_string(m_currentConfig.height) + ", Fullscreen: " + std::to_string(m_currentConfig.fullscreen) + ", Title: " + m_currentConfig.title);

		if (getApp(true) != nullptr)
			Error("Cannot create two NF Application objects!");
		setApp(this);
		m_hInst = GetModuleHandle(NULL);
		registerWindowClass();
		RECT windowSize = getWindowRect();
		int x = 0;
		int y = 0;
		calculateNewWindowPos(x, y);
		m_window = CreateWindowEx(NULL, m_wclassName, toWide(m_currentConfig.title).data(), WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, x, y, windowSize.right, windowSize.bottom, NULL, NULL, m_hInst, NULL);
		m_defaultWindowStyle = GetWindowLong(m_window, GWL_STYLE);
		SetProp(m_window, L"App", this);
		if (m_currentConfig.fullscreen) toggleFullscreen();
	}

	void Application::setWindowIcon(HANDLE hIcon) {
		m_customWindowIconSet = true;
		SendMessage(m_window, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(m_window, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}
	//TODO: Test these top-level features
	void Application::setWindowCursor(HCURSOR hCursor) {
		SetClassLongPtr(m_window, GCLP_HCURSOR, (LONG_PTR)hCursor);
	}

	AudioEngine* Application::getAudioEngine() const {
		return m_audio;
	}

	PhysicsEngine* Application::getPhysicsEngine() const {
		return m_physics;
	}

	void Application::addState(Gamestate* state, const std::string& stateName) {
		if (m_states.find(stateName) == m_states.end()) {
			m_states[stateName] = state;
		}
		else
			Error("State \"" + (std::string)stateName + (std::string)"\" already exists!");
	}

	void Application::setDefaultState(const std::string& stateName) {
		if (!m_defaultStateAdded) {
			if (m_states.find(stateName) != m_states.end()) {
				m_defaultState = stateName;
				m_defaultStateAdded = true;
			}
			else
				Error("State \"" + (std::string)stateName + (std::string)"\" doesn't exist!");
		}
		else
			Error("More than one default state defined!");
	}

	const std::string& Application::getDefaultState() {
		return m_defaultState;
	}

	void Application::run() {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Input Thread");
#endif
		showWindow(true);
		m_running = true;
		MSG msg = { };
		std::thread mainThread(&Application::runMainGameThread, this);
		while (m_running) {
			if (m_quit)
				PostMessage(m_window, WM_CLOSE, NULL, NULL);
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) {
					m_running = false;
					break;
				}
			}
			updateMouse();
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		mainThread.join();
	}

	bool Application::isCustomWindowIcon() {
		return m_customWindowIconSet;
	}

	void Application::changeState(const std::string& stateName) {
		if (m_states.find(stateName) == m_states.end())
			Error("State \"" + (std::string)stateName + (std::string)"\" doesn't exist!");
		m_stateChange = true;
		m_nextState = stateName;
	}

	Gamestate* Application::getCurrentState() {
		return m_currentState;
	}

	void Application::showWindow(bool show) {
		if (show)
			ShowWindow(m_window, SW_SHOW);
		else
			ShowWindow(m_window, SW_HIDE);
	}

	const HWND& Application::getWindow() {
		return m_window;
	}
	//TODO: Throughly test this
	void Application::changeConfig(const Config& in) {
		SetWindowText(m_window, toWide(in.title).data());
		bool temp = m_currentConfig.fullscreen;
		m_currentConfig = in;
		if (in.fullscreen != temp)
			toggleFullscreen();
		if (m_currentConfig.fullscreen)
			return;
		int x = 0, y = 0;
		calculateNewWindowPos(x, y);
		RECT size = getWindowRect();
		SetWindowPos(m_window, HWND_TOP, x, y, size.right, size.bottom, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	const Config& Application::getConfig() const {
		return m_currentConfig;
	}

	int Application::getFPS() const {
		return m_FPS;
	}

	bool Application::isKeyHeld(unsigned int code) {
		if (code > 7 && code < 164 && GetForegroundWindow() == m_window)
			return GetKeyState(code) & 0x8000;
		return false;
	}

	bool Application::isKeyPressed(unsigned int code) {
		if (code > 7 && code < 164) {
			if (m_keysPressed[code]) {
				m_keysPressed[code] = false;
				return true;
			}
		}
		return false;
	}

	bool Application::isMouse(unsigned int code) {
		if (code < 7)
			return GetKeyState(code) & 0x8000;
		return false;
	}

	void Application::trackMouse(bool track) {
		m_trackingMouse = track;
		if (m_trackingMouse == true)
			m_mouseTrackFirst = true;
	}

	void Application::getMouseDiff(int& x, int& y) {
		x = m_mouseDiffX;
		y = m_mouseDiffY;
		m_mouseDiffX = 0;
		m_mouseDiffY = 0;
	}

	Vec2 Application::getMousePos() {
		return Vec2(m_mouseX, m_mouseY);
	}

	Application* Application::getApp(bool first) {
		if (!currentApp && !first)
			Error("No Application has been created yet!");
		return currentApp;
	}

	void Application::registerWindowClass() {
		if (!FindWindow(L"NFClass", NULL)) {
			m_wclassName = L"NFClass";
			WNDCLASS wclass = { };
			wclass.lpszClassName = m_wclassName;
			wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
			wclass.hIcon = NULL;
			wclass.hInstance = m_hInst;
			wclass.lpfnWndProc = Application::WindowProc;
			RegisterClass(&wclass);
		}
		else
			Error("Cannot run multiple NF applications concurrently!");
	}

	RECT Application::getWindowRect() const {
		int w = m_currentConfig.width;
		int h = m_currentConfig.height;
		RECT temp = { };
		temp.right = w;
		temp.bottom = h;
		AdjustWindowRect(&temp, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, false);
		temp.right -= temp.left;
		temp.bottom -= temp.top;
		return temp;
	}

	void Application::calculateNewWindowPos(int& x, int& y) {
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(m_window, MONITOR_DEFAULTTOPRIMARY), &mi);
		int monX = (mi.rcWork.right - mi.rcWork.left) / 2;
		int monY = (mi.rcWork.bottom - mi.rcWork.top) / 2;

		x = monX - (m_currentConfig.width / 2);
		y = monY - (m_currentConfig.height / 2);
	}

	void Application::toggleFullscreen() {
		DWORD wndStyle = GetWindowLong(m_window, GWL_STYLE);
		if (wndStyle & WS_OVERLAPPEDWINDOW) {
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(MonitorFromWindow(m_window, MONITOR_DEFAULTTOPRIMARY), &mi);
			SetWindowLong(m_window, GWL_STYLE, wndStyle & ~WS_OVERLAPPEDWINDOW);
			m_currentConfig.width = mi.rcMonitor.right - mi.rcMonitor.left;
			m_currentConfig.height = mi.rcMonitor.bottom - mi.rcMonitor.top;
			SetWindowPos(m_window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, m_currentConfig.width, m_currentConfig.height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
		else {
			SetWindowLong(m_window, GWL_STYLE, wndStyle | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX);
			SetWindowPos(m_window, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	void Application::updateMouse() {
		POINT mouse;
		GetCursorPos(&mouse);
		ScreenToClient(m_window, &mouse);
		if (GetFocus() == m_window) {
			m_mouseX = mouse.x;
			m_mouseY = mouse.y;
			if (m_mouseX > m_currentConfig.width)
				m_mouseX = m_currentConfig.width;
			if (m_mouseX < 0)
				m_mouseX = 0;
			if (m_mouseY > m_currentConfig.height)
				m_mouseY = m_currentConfig.height;
			if (m_mouseY < 0)
				m_mouseY = 0;

			if (m_trackingMouse && m_currentState->isRunning()) {
				int middleX = m_currentConfig.width / 2;
				int middleY = m_currentConfig.height / 2;
				m_mouseDiffX += m_mouseX - middleX;
				m_mouseDiffY += middleY - m_mouseY;
				if (m_mouseTrackFirst) {
					m_mouseDiffX = 0;
					m_mouseDiffY = 0;
					m_mouseTrackFirst = false;
				}
				POINT middle = { middleX, middleY };
				ClientToScreen(m_window, &middle);
				SetCursorPos(middle.x, middle.y);
			}
		}
	}

	void Application::quit() {
		m_quit = true;
	}

	void Application::runMainGameThread() {
#ifdef _DEBUG
		SetThreadDescription(GetCurrentThread(), L"Main Engine Thread");
#endif
		m_renderer = new Renderer(this);
		m_audio = new AudioEngine(this);
		m_physics = new PhysicsEngine(this);
		Gamestate* sIntro = new IntroGamestate;
		m_currentState = sIntro;
		m_currentState->run(this);
		m_renderer->setFade(true, false, true);
		std::chrono::steady_clock::time_point currentFrame = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();
		while (m_running) {
			currentFrame = std::chrono::steady_clock::now();
			m_deltaTime = std::chrono::duration<double>(currentFrame - lastFrame).count();
			if (m_deltaTime >= m_minFrametime) {
				lastFrame = std::chrono::steady_clock::now();
				//Should the physics update before user code?
				m_physics->update(m_deltaTime);
				m_currentState->update(m_deltaTime);
				m_currentState->render(*m_renderer);
				m_renderer->doFrame(m_currentState->getCamera(), m_deltaTime);
				if (m_stateChange)
					doStateChange();
				m_fpsClock2 = std::chrono::steady_clock::now();
				m_fpsDuration = m_fpsClock2 - m_fpsClock1;
				if (m_fpsDuration.count() >= 0.2) {
					m_FPS = (int)std::round(1.0 / m_deltaTime);
#ifdef _DEBUG
					static int i = 0;
					i++;
					if (i % 5 == 0)
						Log("FPS: " + std::to_string(m_FPS));
#endif
					m_fpsClock1 = std::chrono::steady_clock::now();
				}
			}
		}
		m_audio->stopAllSounds();
		m_currentState->stop();
		delete sIntro;
		delete m_physics;
		delete m_audio;
		delete m_renderer;
	}

	void Application::doStateChange() {
		static bool once = true;
		if (once) {
			m_renderer->setFade(false, true, false);
			once = false;
		}

		if (m_renderer->isFadeOutComplete()) {
			m_audio->stopAllSounds();
			m_currentState->stop();
			m_currentState = m_states[m_nextState];
			m_currentState->run(this);
			m_renderer->setFade(true, false, false);
			m_stateChange = false;
			once = true;
		}
	}

	void Application::setApp(Application* app) {
		currentApp = app;
	}

	LRESULT CALLBACK Application::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Application* app = (Application*)GetProp(hWnd, L"App");
		switch (uMsg) {
			case WM_CREATE: {
				return 0;
			}
			case WM_SYSKEYDOWN: {
				if (GetKeyState(VK_RETURN) & 0x8000) {
					if (!(lParam & (1 << 30))) {
						if (!app->m_currentConfig.fullscreen) {
							app->m_altWidth = app->m_currentConfig.width;
							app->m_altHeight = app->m_currentConfig.height;
						}
						app->changeConfig({ app->m_currentConfig.width, app->m_currentConfig.height, !app->m_currentConfig.fullscreen, app->m_currentConfig.title });
						if (!app->m_currentConfig.fullscreen) {
							app->changeConfig({ app->m_altWidth, app->m_altHeight, app->m_currentConfig.fullscreen, app->m_currentConfig.title });
						}
					}
					return 0;
				}
				break;
			}
			case WM_MENUCHAR: {
				return MNC_CLOSE << 16;
			}
			case WM_KEYDOWN: {
				if (wParam < 164 && !(lParam & (1 << 30)) && GetFocus() == hWnd)
					app->m_keysPressed[wParam] = true;
				return 0;
			}
			case WM_KEYUP: {
				if (wParam < 164 && GetFocus() == hWnd)
					app->m_keysPressed[wParam] = false;
				return 0;
			}
			case WM_SETCURSOR: {
				if (LOWORD(lParam) != HTCLIENT)
					break;
				if (app->m_trackingMouse && app->m_currentState->isRunning() && LOWORD(lParam) == HTCLIENT && GetFocus() == hWnd) {
					SetCursor(NULL);
					return 0;
				}
				break;
			}
			case WM_CLOSE: {
				DestroyWindow(hWnd);
				return 0;
			}
			case WM_DESTROY: {
				PostQuitMessage(0);
				return 0;
			}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	Application::~Application() {
		Log("Exiting NF application");

		for (std::pair<std::string, Gamestate*> state : m_states) {
			Gamestate* curr = state.second;
			delete curr;
		}
	}

	Application* Application::currentApp = nullptr;
}
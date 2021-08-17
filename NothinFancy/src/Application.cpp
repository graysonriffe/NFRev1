#include "Application.h"
#include "Utility.h"
#ifdef NFENGINE
#include "GL\glew.h"
#include "GL\wglew.h"
#endif

namespace nf {
	DEBUGINIT;

	Application::Application(Config& config) :
		m_currentConfig(config),
		m_wndPlacement{ sizeof(m_wndPlacement) },
		m_running(false)
	{
		Log("Creating NF application");
		Log("Width: " + std::to_string(m_currentConfig.width) + ", Height: " + std::to_string(m_currentConfig.height) + ", Fullscreen: " + std::to_string(m_currentConfig.fullscreen) + ", Title: " + m_currentConfig.title);

		m_hInst = GetModuleHandle(NULL);
		registerWindowClass();
		RECT windowSize = getWindowRect();

		m_window = CreateWindowEx(NULL, m_wclassName, toWide(m_currentConfig.title), WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 1280, windowSize.bottom, NULL, NULL, m_hInst, NULL);
		m_defaultWindowStyle = GetWindowLong(m_window, GWL_STYLE);
		SetProp(m_window, L"App", this);
		if (m_currentConfig.fullscreen) toggleFullscreen();

		createOpenGLContext();
		m_states.reserve(100);
		m_activeStates.reserve(100);
		m_sIntro = new IntroGamestate;
		addState(m_sIntro);
	}

	void Application::setWindowIcon(HANDLE hIcon) {
		SendMessage(m_window, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(m_window, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}
	//TODO: Test these top-level features
	void Application::setWindowCursor(HCURSOR hCursor) {
		SetClassLongPtr(m_window, GCLP_HCURSOR, (LONG_PTR)hCursor);
	}

	void Application::addState(IGamestate* in) {
		(*in).onEnter();
		m_states.push_back(in);
	}

	void Application::addDefaultState(IGamestate* in) {
		if (!m_defaultStateAdded) {
			(*in).onEnter();
			m_activeStates.push_back(in);
			m_defaultStateAdded = true;
		}
		else {
			Error("More than one default state defined"); //TODO: Test this
		}
	}

	void Application::run() {
		showWindow(true);
		m_running = true;
		MSG msg = { };
		while (m_running) {
			m_fpsDuration = std::chrono::steady_clock::now() - m_frameClock;
			if (m_fpsDuration.count() >= m_minFrametime) {
				m_deltaTime = m_fpsDuration.count();
				while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					if (msg.message == WM_QUIT)
						m_running = false;
				}
				glClear(GL_COLOR_BUFFER_BIT);
				SwapBuffers(m_hdc);
				m_frames++;
				m_frameClock = std::chrono::steady_clock::now();
				//TODO: Update and render current state
			}
			m_fpsClock2 = std::chrono::steady_clock::now();
			m_fpsDuration = m_fpsClock2 - m_fpsClock1;
			if (m_fpsDuration.count() >= 1.0) {
				m_fpsClock1 = std::chrono::steady_clock::now();
				m_FPS = m_frames;
				m_frames = 0;
			}
		}
	}

	void Application::showWindow(bool show) {
		if (show)
			ShowWindow(m_window, SW_SHOW);
		else
			ShowWindow(m_window, SW_HIDE);
	}

	const Config& Application::getConfig() const {
		return m_currentConfig;
	}//Test this

	int Application::getFPS() const {
		return m_FPS;
	}

	void Application::registerWindowClass() {
		m_wclassName = L"NFClass";
		WNDCLASS wclass = { };
		wclass.lpszClassName = m_wclassName;
		wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wclass.hIcon = NULL;
		wclass.hInstance = m_hInst;
		wclass.lpfnWndProc = Application::WindowProc;
		RegisterClass(&wclass);
	}

	void Application::toggleFullscreen() {
		DWORD wndStyle = GetWindowLong(m_window, GWL_STYLE);
		if (wndStyle & WS_OVERLAPPEDWINDOW) {
			GetWindowPlacement(m_window, &m_wndPlacement);
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(MonitorFromWindow(m_window, MONITOR_DEFAULTTOPRIMARY), &mi);
			SetWindowLong(m_window, GWL_STYLE, wndStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(m_window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
		else {
			SetWindowPlacement(m_window, &m_wndPlacement);
			SetWindowPos(m_window, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			SetWindowLong(m_window, GWL_STYLE, m_defaultWindowStyle);
		}
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

	LRESULT CALLBACK Application::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Application* app = (Application*)GetProp(hWnd, L"App");
		switch (uMsg) {
		case WM_CREATE: {

			return 0;
		}
		case WM_SYSKEYDOWN: {
			if (GetKeyState(VK_RETURN) & 0x8000) {
				app->toggleFullscreen();
				return 0;
			}
			break;
		}
		case WM_MENUCHAR: {
			return MNC_CLOSE << 16;
		}
		case WM_CLOSE: {
			//State onExit() in order
			//unload anything else
			DestroyWindow(hWnd);
			return 0;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);//TODO: Fill out events
	}

	void Application::createOpenGLContext() {
		m_hdc = GetDC(m_window);
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};
		int pf = ChoosePixelFormat(m_hdc, &pfd);
		SetPixelFormat(m_hdc, pf, &pfd);
		m_hglrc = wglCreateContext(m_hdc);
		wglMakeCurrent(m_hdc, m_hglrc);
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			Error("Could not initialize GLEW");
		}
		const int attrib[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0, 0
		};
		wglDeleteContext(m_hglrc);
		m_hglrc = wglCreateContextAttribsARB(m_hdc, NULL, attrib);
		wglMakeCurrent(m_hdc, m_hglrc);
		Log("OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	Application::~Application() {
		//TODO: Iterate through m_activeStates and m_states and exit and unload them
	}
}

//TODO: Rework this file to only contain functions the frontend will need to access

#pragma once
#include <chrono>
#include <unordered_map>
#include <Windows.h>

#include "Config.h"
#include "IntroGamestate.h"

namespace nf {
	class Drawable;
	class Shader;
	class Model;

	class Entity {
		struct Vec3 {
			Vec3(float x1) : x(x1), y(x1), z(x1) {}
			Vec3(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {}
			float x, y, z;
		};
	public:
		Entity();

		void create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount);
		void setLocation(float x, float y, float z);
		void setRotation(float x, float y, float z);
		void setScale(float x);
		void setScale(float x, float y, float z);

		void bind(Shader* shader);
		Model* getModel() const;

		~Entity();
	private:
		void setModelMatrix(Shader* shader);

		Model* m_model;

		Vec3 m_position;
		Vec3 m_rotation;
		Vec3 m_scale;
	};

	class Renderer {
	public:
		Renderer(Application* app);

		void render(Entity& in);

		void doFrame();

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		std::vector<Drawable*> m_lGame;
		std::vector<Drawable*> m_lUI;
		const char* m_defaultVertex;
		const char* m_defaultFragment;
		Shader* m_defaultShader;
	};

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
		int m_altWidth, m_altHeight;

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
		std::unordered_map<std::string, Gamestate*> m_states;
		Gamestate* m_sIntro;
		Gamestate* m_DefaultState;
		bool m_defaultStateAdded;
		Gamestate* m_currentState;
		bool m_stateChange;
		std::string m_nextState;

		//Array of booleans that represent keyboard and mouse input minus the scrollwheel
		bool m_input[164];

		//Renderer object to use OpenGL to render the current state
		Renderer* m_renderer;
	};
}
#include "Input.h"
#include "Utility.h"
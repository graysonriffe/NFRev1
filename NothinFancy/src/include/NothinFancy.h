//TODO: Rework this file to only contain functions the frontend will need to access

#pragma once
#include <chrono>
#include <unordered_map>
#include <Windows.h>

#include "Config.h"
#include "Utility.h"
#include "IntroGamestate.h"
#include "Assets.h"
#include "Light.h"
#include "Cubemap.h"
#include "Text.h"
#include "UITexture.h"
#include "Input.h"

namespace nf {
	class Drawable;
	class Shader;
	class Model;

	class Entity {
	public:
		Entity();

		void create(Asset* modelAsset, Asset* textureAsset = nullptr);

		void setPosition(double x, double y, double z);
		void setPosition(const Vec3& position);
		void setRotation(double x, double y, double z);
		void setRotation(const Vec3& rotation);
		void setScale(double x);
		void setScale(double x, double y, double z);
		void setScale(const Vec3& scale);

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
		void render(UIElement& in);
		void render(Light& in);
		void render(Cubemap& in);

		void doFrame(Camera* camera);

		~Renderer();
	private:
		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		AssetPack m_baseAP;

		std::vector<Light*> m_lights;
		std::vector<Entity*> m_lGame;
		Cubemap* m_cubemap;
		std::vector<UIElement*> m_lUI;
		Shader* m_entityShader;
		Shader* m_textShader;
		Shader* m_uiTextureShader;
		Shader* m_cubemapShader;
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
		const std::string& getDefaultState();
		void run();
		void changeState(const std::string& stateName);
		void showWindow(bool show);
		const HWND& getWindow();
		void changeConfig(const Config& in);
		const Config& getConfig() const;
		int getFPS() const;
		bool isInput(unsigned int code);
		void trackMouse(bool track);
		void getMouseDiff(int& x, int& y);

		void quit();
		~Application();
	private:
		void registerWindowClass();
		RECT getWindowRect() const;
		void calculateNewWindowPos(int& x, int& y);
		void toggleFullscreen();
		void updateInput();
		void runMainGameThread();
		void startIntroState();
		void doStateChange();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
		int m_frames;
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
		bool m_input[164];
		unsigned int m_mouseX, m_mouseY;
		bool m_trackingMouse;
		bool m_mouseTrackFirst;
		int m_mouseDiffX, m_mouseDiffY;

		//Renderer object to use OpenGL to render the current state
		Renderer* m_renderer;
	};
}
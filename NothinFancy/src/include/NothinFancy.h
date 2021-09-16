//TODO: Rework this file to only contain functions the frontend will need to access

#pragma once
#include <chrono>
#include <unordered_map>
#include <array>
#include <Windows.h>

#include "Config.h"
#include "Utility.h"
#include "IntroGamestate.h"
#include "Assets.h"
#include "Entity.h"
#include "Light.h"
#include "Cubemap.h"
#include "Text.h"
#include "UITexture.h"
#include "Button.h"
#include "Sound.h"
#include "Input.h"

namespace nf {
	class Drawable;
	class Shader;
	class Model;
	class AudioEngine;

	class Renderer {
	public:
		Renderer(Application* app);

		void setFade(bool in, bool out, bool noText);
		bool isFadeOutComplete();

		void render(Entity& in);
		void render(UIElement& in);
		void render(Light& in);
		void render(Cubemap& in);

		void doFrame(Camera* camera, double dT);

		~Renderer();
	private:
		void renderShadowMaps(unsigned int startingLight, unsigned int count);

		void loadBaseAssets();
		void createShadowMap();

		Application* m_app;

		HDC m_hdc;
		HGLRC m_hglrc;

		AssetPack m_baseAP;

		unsigned int m_shadowMapFBO;
		int m_directionalDepthTexSize;
		int m_pointDepthTexSize;
		std::vector<unsigned int> m_directionalShadowMaps;
		std::vector<unsigned int> m_pointShadowMaps;
		unsigned int m_texSlots;

		std::vector<Light*> m_lights;
		std::vector<Entity*> m_lGame;
		Cubemap* m_cubemap;
		std::vector<UIElement*> m_lUI;
		Shader* m_entityShader;
		Shader* m_textShader;
		Shader* m_uiTextureShader;
		Shader* m_cubemapShader;
		Shader* m_fadeShader;
		Shader* m_directionalShadowShader;
		Shader* m_pointShadowShader;

		bool m_fadeIn, m_fadeOut;
		bool m_fadeNoText;
		bool m_fadeOutComplete;
		Text m_loadingText;
		VertexArray* m_fadeVAO;
		IndexBuffer* m_fadeIB;
	};

	class Application {
	public:
		Application(Config& conf);
		Application() = delete;
		Application(Application& other) = delete;

		void setWindowIcon(HANDLE hIcon);
		void setWindowCursor(HCURSOR hCursor);
		void addState(Gamestate* state, const std::string& stateName);
		void addDefaultState(const std::string& stateName);
		const std::string& getDefaultState();
		void run();
		bool isCustomWindowIcon();
		void changeState(const std::string& stateName);
		Gamestate* getCurrentState();
		void showWindow(bool show);
		const HWND& getWindow();
		void changeConfig(const Config& in);
		const Config& getConfig() const;
		int getFPS() const;
		bool isKeyHeld(unsigned int code);
		bool isKeyPressed(unsigned int code);
		bool isMouse(unsigned int code);
		void trackMouse(bool track);
		void getMouseDiff(int& x, int& y);
		Vec2 getMousePos();
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
		bool m_customWindowIconSet;
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
		std::array<bool, 164> m_keysPressed;
		unsigned int m_mouseX, m_mouseY;
		bool m_trackingMouse;
		bool m_mouseTrackFirst;
		int m_mouseDiffX, m_mouseDiffY;

		//Renderer object to use OpenGL to render the current state
		Renderer* m_renderer;

		AudioEngine* m_audio;
	};
}
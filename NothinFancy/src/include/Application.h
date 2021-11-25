#pragma once
#include <chrono>
#include <unordered_map>
#include <array>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Config.h"
#include "Renderer.h"
#include "Gamestate.h"
#ifndef NFIMPL
#include "IntroGamestate.h"
#include "AudioEngine.h"
#include "PhysicsEngine.h"
#endif
//TODO: Document ALL frontend functions in new include files for the frontend only

namespace nf {
	class AudioEngine;
	class PhysicsEngine;

	/**
	 * @brief Main class
	 * 
	 * This class handles low-level Windows APIs and provides a window.
	 * Every NF applicaiton will create one.
	*/
	class Application {
	public:
		Application(Application& other) = delete;
		/**
		 * @brief Main constructor
		 * @param conf Configuration to be applied to the app as default. Must be provided.
		*/
		Application(Config& conf);
		/**
		 * @brief Sets the app's window icon
		 * @param hIcon Valid HICON loaded with a Windows API funciton like LoadIcon
		 * 
		 * This also sets the window's taskbar icon.
		*/
		void setWindowIcon(HICON hIcon);
		/**
		 * @brief Sets the app's cursor
		 * @param hCursor Valid HCURSOR loaded with a Windows API funciton like LoadCursor
		*/
		void setWindowCursor(HCURSOR hCursor);
		/**
		 * @brief Adds a Gamestate to the app's available gamestates
		 * @param state Pointer to the Gamestate instance
		 * @param stateName Name of the state to be used later
		 * 
		 * Gamestates can only be accessed in an app if this funciton has been called.
		 * The stateName is added to the app's list of states that can be switched to later.
		*/
		void addState(Gamestate* state, const std::string& stateName);
		/**
		 * @brief Sets the state the app should switch to after the logo gamestate
		 * @param stateName Name of a priviously-added state
		 * 
		 * @note The gamestate must have already been added to the app by calling addState.
		*/
		void setDefaultState(const std::string& stateName);
		/**
		 * @brief Queries the name of the current gamestate
		 * @return Name of current gamestate
		*/
		const std::string& getDefaultState();
		/**
		 * @brief Runs the application
		 * 
		 * This funciton initializes and runs the main loop on the calling thread.
		 * 
		 * @note This function will not return untill the engine exits.
		*/
		void run();
		/**
		 * @brief Change to a different gamestate
		 * @param stateName name of the gamestate to switch to
		 * 
		 * This function starts the process of unloading the current gamestate and loading
		 * a new one.
		 * 
		 * @note The actual gamestate switching does not happen immediately before the next
		 * frame, but instead after a short amount of time for the engine to fade to a
		 * loading screen.
		*/
		void changeState(const std::string& stateName);
		/**
		 * @brief Sets the visibility of the main window
		 * @param show Show the window?
		 * 
		 * This function can be called at any time, regardless of the state of the application.
		*/
		void showWindow(bool show);
		/**
		 * @brief Changes the app's config
		 * @param in The configuration to replace the current one
		 * 
		 * This function reads in the new Config and sets up the engine  to reflect it.
		 * 
		 * @sa Config
		*/
		void changeConfig(const Config& in);
		/**
		 * @brief Queries the current configuration
		 * @return A const reference to the current configuration of the app
		 * 
		 * @sa Config
		*/
		const Config& getConfig() const;
		/**
		 * @brief Queries the current FPS
		 * @return The current FPS
		 * 
		 * @todo Start averaging the FPS instead of calculating it every frame.
		*/
		int getFPS() const;
		/**
		 * @brief Queries if a certain key is currently down
		 * @param code An NFI code representing a keyboard key
		 * @return If the key is down
		 * 
		 * This function returns true for every frame that a key is held down for. This is
		 * perfect for continuous movement or really anything that involves holding keys.
		*/
		bool isKeyHeld(unsigned int code);
		/**
		 * @brief Queries if a certain key has been pressed
		 * @param code An NFI code representing a keyboard key
		 * @return If the key has been pressed
		 * 
		 * This function returns true for only one frame that a key is held down for.
		 * This is perfect for toggling menus, actions, etc.
		*/
		bool isKeyPressed(unsigned int code);
		/**
		* @brief Queries if a certain mouse button is currently down
		 * @param code An NFI code representing a mouse button
		 * @return If the mouse button is down
		 * 
		 * This function returns true for every frame that a mouse button is held down for.
		 * This is perfect for automatic weapons, dragging objects, etc.
		*/
		bool isMouseHeld(unsigned int code);
		/**
		* @brief Queries if a certain mouse button has been pressed
		 * @param code An NFI code representing a mouse button
		 * @return If the mouse button has been pressed
		 * 
		 * This function returns true for only one frame that a mouse button is held down for.
		 * This is perfect for semi-automatic weapons, actions, etc.
		*/
		bool isMouseClicked(unsigned int code);
		/**
		 * @brief Queries the cursor's current position in the window
		 * @return The coordinates of the cursor in window space
		 * 
		 * A return value of (0, 0) represents the top-left corner of the window while
		 * a return value of (window width, window height) represents the bottom-right
		 * corner of the window.
		*/
		Vec2 getMousePos();
		/**
		 * @brief Exits the main loop and cleans up the engine
		 * 
		 * @todo Free up the possibility for an implementation to create another app
		 * before it exits.
		*/
		void quit();
#ifndef NFIMPL
		Renderer* getRenderer() const;
		AudioEngine* getAudioEngine() const;
		PhysicsEngine* getPhysicsEngine() const;
		bool hasCustomWindowIcon();
		Gamestate* getCurrentState();
		const HWND& getWindow();
		void trackMouse(bool track);
		void getMouseDiff(int& x, int& y);
		static Application* getApp(bool first = false);
#endif
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
		HICON m_windowIcon;
		unsigned int m_altWidth, m_altHeight;

		std::chrono::duration<float> m_fpsDuration;
		float m_deltaTime;
		std::chrono::steady_clock::time_point m_fpsClock1 = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point m_fpsClock2 = m_fpsClock1;
		const int m_targetFPS = 60;
		const float m_minFrametime = 1.0f / m_targetFPS;
		int m_FPS;

		//Inactive states to potentially be active during the Application's lifetime
		//Mapped to const char* to be referenced later in the frontend
		std::unordered_map<std::string, Gamestate*> m_states;
		std::string m_defaultState;
		bool m_defaultStateAdded;
		Gamestate* m_currentState;
		bool m_stateChange;
		Gamestate* m_nextState;
		bool m_stateChangeStarted;

		//Array of booleans that represent keyboard and mouse input minus the scrollwheel
		std::array<bool, 164> m_inputPressed;
		unsigned int m_mouseX, m_mouseY;
		bool m_trackingMouse;
		bool m_mouseTrackFirst;
		int m_mouseDiffX, m_mouseDiffY;

		//Renderer object to use OpenGL to render the current state
		Renderer* m_renderer;

		//Handles nf::Sound objects and calculates 3D sounds
		AudioEngine* m_audio;

		//Calculates physics
		PhysicsEngine* m_physics;
	};
}
#pragma once

namespace nf {
	/**
	 * @brief Configuration for NF
	 * 
	 * This struct contains basic information about the engine's window.
	 * 
	 * @todo Add more things here such as mouse sensativity and FPS target?
	*/
	struct Config {
	public:
		/**
		 * @brief Width of the engine window (should fullscreen be false)
		*/
		unsigned int width;
		/**
		 * @brief Height of the engine window (should fullscreen be false)
		*/
		unsigned int height;
		/**
		 * @brief Sets fullscreen
		 * 
		 * If fullscreen, the engine will be displayed at the current monitor's
		 * resolution.
		*/
		bool fullscreen;
		/**
		 * @brief Title of the engine's window
		*/
		const char* title;
	};
}
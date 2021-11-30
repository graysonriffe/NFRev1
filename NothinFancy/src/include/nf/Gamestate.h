#pragma once
#include <vector>
#include <unordered_set>

#include "nf/Camera.h"
#include "nf/NFObject.h"

namespace nf {
	class Application;
	class Renderer;
	class Camera;
	class Entity;
	class Model;
	class Texture;

	/**
	 * @brief A state NF can be in that includes a collection of objects and user-defined
	 * behavior
	 * 
	 * Every user-defined state inherits from this class.
	*/
	class Gamestate {
	public:
		//TODO: Add this to other objects
		Gamestate(const Gamestate& other) = delete;
		/**
		 * @brief Default constructor; Must be used
		*/
		Gamestate();
		/**
		 * @brief The state's 'constructor'
		 * 
		 * This function runs when the state is started.
		*/
		virtual void onEnter();
		/**
		 * @brief Update function
		 * @param deltaTime Amount of time the previous frame took to produce in seconds
		 * 
		 * This function is called every frame. It is called before render.
		 * 
		 * The deltaTime parameter's purpose is to create non-frame-dependant gameplay. This
		 * number should be multiplied with user numbers likes velocities. Doing this will
		 * produce output that will look the same on higher and lower framerates.
		*/
		virtual void update(float deltaTime);
		/**
		 * @brief Render function
		 * @param renderer A reference to the Renderer object
		 * 
		 * This function will contain calls to Renderer::render to decide what will be drawn
		 * on the next frame.
		 * 
		 * @sa Renderer
		*/
		virtual void render(Renderer& renderer);
		/**
		 * @brief The state's 'destructor'
		 * 
		 * This function runs when the state is exited.
		 * 
		 * It is imperitive that the class members are 'reset' back to their default values
		 * here. This does not include members of the Gamestate that are NF objects
		 * such as entites and UIElements.
		 * 
		 * For more information see the @ref gamestates.
		*/
		virtual void onExit();
		/**
		 * @brief Returns the state's Camera
		 * @return The state's camera
		*/
		Camera* getCamera();
		/**
		 * @brief Sets the state's ambient light level
		 * @param stength Light level multiplier; Defaults to 0.1f
		 * 
		 * An ambient light level of 0.0f is perfect for pitch dark rooms where no light means
		 * completely black while a level of 1.0f means full-bright.
		*/
		void setAmbientLight(float stength);
		/**
		 * @brief Sets the state's gravity in a certain direction
		 * @param gravity A vector representing a direction the force of gravity should act in
		 * In units of Earth gravity
		 * 
		 * The default gravity of a state is:
		 * 
		 * ~~~
		 * Vec3(0.0, -1.0, 0.0);
		 * ~~~
		*/
		void setGravity(const Vec3& gravity);
		/**
		 * @brief Sets the state's gravity downward
		 * @param strength A multiplier in units of Earth gravity
		 * 
		 * The default is 1.0f;
		*/
		void setGravity(float strength);
#ifndef NFIMPL
		void run(Application* app, bool physics = true);
		bool isRunning();
		bool isLoading();
		void stop();
#endif
		//TODO: Probably change these to private members?
		std::vector<NFObject*> m_nfObjects;
		std::vector<Entity*> m_entsToDelete;
		std::unordered_set<Model*> m_modelsToDelete;
		std::unordered_set<Texture*> m_texturesToDelete;
	protected:
		Application* app;
		Camera* camera;
		bool m_loading;
		bool m_running;
	};
}
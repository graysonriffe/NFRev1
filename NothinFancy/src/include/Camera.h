#pragma once
#include"Utility.h"

namespace nf {
	class Application;
	class Shader;
	//TODO: Make sure there are always newlines here;

	/**
	 * @brief The viewpoint for the engine
	*/
	class Camera {
	public:
		/**
		 * @brief Dictates the behavior of a camera's movement
		*/
		enum class Type {
			/**
			 * @brief No mouse capture
			 * 
			 * Perfect for selecting menus with the mouse
			*/
			UI,
			/**
			 * @brief First-person mouse-look movement
			*/
			FIRST_PERSON,
			/**
			 * @brief Orbit movement around either a position or Entity
			*/
			ORBIT,
			/**
			 * @brief Fixed to either a position or Entity
			*/
			FIXED
		};

#ifndef NFIMPL
		Camera(Application* app);
#else
		Camera() = delete;
		Camera(const Camera& other) = delete;
#endif
		/**
		 * @brief Sets the camera's type
		 * @param cameraType the @ref Type to change to
		*/
		void setType(Type cameraType);
		/**
		 * @brief Queries the current @ref Type of the camera
		 * @return The current Type of the camera
		*/
		Type getType() const;
		/**
		 * @brief Moves the camera given an offset
		 * @param offset Offset vector
		 * 
		 * The x component of the offset is added to the x coordinate of the camera
		 * while the y component is added to the z coordinate.
		*/
		void move(const Vec2& offset);
		/**
		 * @brief Moves the camera forwards and backwards
		 * @param offset The amount to move the camera in the Z direction
		*/
		void moveZ(float offset);
		/**
		 * @brief Moves the camera left and right
		 * @param offset The amount to move the camera in the X direction
		*/
		void moveX(float offset);
		/**
		 * @brief Sets the position of the camera with three floats
		 * @param x X component
		 * @param y Y component
		 * @param z Z component
		*/
		void setPosition(float x, float y, float z);
		/**
		 * @brief Sets the position of the camera with a Vec3
		 * @param position Position vector
		*/
		void setPosition(const Vec3& position);
		/**
		 * @brief Queries the position of the camera
		 * @return The position of the camera
		*/
		const Vec3& getPosition();
		/**
		 * @brief Sets the rotation of the camera given a pitch and yaw
		 * @param yaw Left and right rotation in degrees
		 * @param pitch Up and down rotation in degrees
		*/
		void setRotation(float yaw, float pitch);
		/**
		 * @brief Sets the rotation of the camera given a Vec2
		 * @param rotation Rotation vector
		 * 
		 * The x component of the rotation represents yaw (left and right rotation)
		 * while the y component represents pitch (up and down rotation).
		 * Both are in degrees.
		*/
		void setRotation(const Vec2& rotation);
		/**
		 * @brief Gets the rotation of the camera
		 * @return The rotation of the camera
		*/
		const Vec3& getRotation();
#ifndef NFIMPL
		void update(Shader* gBufferShader, Shader* lightingShader, Shader* cubemapShader);
#endif
		~Camera();
	private:
		Application* m_app;
		Type m_type;

		Vec3 m_position;
		Vec3 m_front;
		float m_yaw;
		float m_pitch;
	};
}
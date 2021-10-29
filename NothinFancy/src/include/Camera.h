#pragma once
#include"Utility.h"

namespace nf {
	class Application;
	class Shader;
	//TODO: Make sure there are always newlines here;
	class Camera {
	public:
		enum class Type {
			UI,
			FIRST_PERSON,
			ORBIT,
			FIXED
		};
		Camera(Application* app);

		void setType(Type cameraType);
		Type getType() const;

		void moveForward(float speed);
		void moveBackward(float speed);
		void moveRight(float speed);
		void moveLeft(float speed);
		void setPosition(float x, float y, float z);
		void setPosition(const Vec3& position);
		const Vec3& getPosition();
		void setRotation(float x, float y);
		void setRotation(const Vec2& rotation);
		const Vec3& getRotation();

		void bind(Shader* gBufferShader, Shader* lightingShader, Shader* cubemapShader);

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
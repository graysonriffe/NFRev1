#pragma once
#include"Utility.h"

namespace nf {
	class Application;
	class Shader;
	//TODO: Make sure there are always newlines here;
	class Camera {
	public:
		enum class Type {
			NF_CAMERA_UI,
			NF_CAMERA_FIRST_PERSON,
			NF_CAMERA_ORBIT,
			NF_CAMERA_FIXED
		};
		Camera(Application* app);

		void setType(Type cameraType);
		Type getType() const;

		void moveForward(double speed);
		void moveBackward(double speed);
		void moveRight(double speed);
		void moveLeft(double speed);
		void setPosition(double x, double y, double z);
		void setPosition(const Vec3& position);

		void bind(Shader* shader);

		~Camera();
	private:
		Application* m_app;
		Type m_type;

		Vec3 m_position;
		Vec3 m_front;
	};
}
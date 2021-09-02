#include "Camera.h"

#include "glm/glm.hpp"

#include "Application.h"
#include "Shader.h"

namespace nf {
	Camera::Camera(Application* app) :
		m_app(app),
		m_type(Type::NF_CAMERA_UI),
		m_position(0.0),
		m_front(0.0, 0.0, -1.0),
		m_yaw(-90.0f),
		m_pitch(0.0)
	{

	}

	void Camera::setType(Type cameraType) {
		if (cameraType != m_type) {
			m_type = cameraType;
			if (m_type == Type::NF_CAMERA_FIRST_PERSON || m_type == Type::NF_CAMERA_ORBIT)
				m_app->trackMouse(true);
			else
				m_app->trackMouse(false);
		}
	}

	Camera::Type Camera::getType() const {
		return m_type;
	}

	void Camera::moveForward(double speed) {
		Vec3 temp = m_front * speed;
		m_position += temp;
	}

	void Camera::moveBackward(double speed) {
		Vec3 temp = m_front * speed;
		m_position -= temp;
	}

	void Camera::moveRight(double speed) {
		glm::vec3 front = { m_front.x, m_front.y, m_front.z };
		glm::vec3 temp = glm::normalize(glm::cross(front, glm::vec3(0.0, 1.0, 0.0))) * (float)speed;
		Vec3 move = { temp.x, temp.y, temp.z };
		m_position += move;
	}

	void Camera::moveLeft(double speed) {
		glm::vec3 front = { m_front.x, m_front.y, m_front.z };
		glm::vec3 temp = glm::normalize(glm::cross(front, glm::vec3(0.0, 1.0, 0.0))) * (float)speed;
		Vec3 move = { temp.x, temp.y, temp.z };
		m_position -= move;
	}

	void Camera::setPosition(double x, double y, double z) {
		m_position = { x, y, z };
	}

	void Camera::setPosition(const Vec3& position) {
		m_position = position;
	}

	void Camera::bind(Shader* shader) {
		glm::mat4 view;

		switch (m_type) {
			case Type::NF_CAMERA_UI: {
				view = glm::mat4(1.0);
				break;
			}
			case Type::NF_CAMERA_FIRST_PERSON: {
				int mouseDiffx = 0;
				int mouseDiffy = 0;
				m_app->getMouseDiff(mouseDiffx, mouseDiffy);
				float mouseX = (float)mouseDiffx * 0.1f;
				float mouseY = (float)mouseDiffy * 0.1f;
				m_yaw += mouseX;
				m_pitch += mouseY;
				if (m_pitch > 89.0f)
					m_pitch = 89.0f;
				if (m_pitch < -89.0f)
					m_pitch = -89.0f;
				glm::vec3 rotation;
				rotation.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
				rotation.y = std::sin(glm::radians(m_pitch));
				rotation.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
				rotation = glm::normalize(rotation);
				m_front = { rotation.x, rotation.y, rotation.z };
				glm::vec3 position(m_position.x, m_position.y, m_position.z);
				glm::vec3 up(0.0, 1.0, 0.0);
				view = glm::lookAt(position, position + rotation, up);
				break;
			}
			case Type::NF_CAMERA_ORBIT: {

				break;
			}
			case Type::NF_CAMERA_FIXED: {

				break;
			}
		}

		shader->setUniform("view", view);
	}

	Camera::~Camera() {

	}
}
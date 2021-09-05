#include "Light.h"

#include "Shader.h"

namespace nf {
	Light::Light() :
		m_type(Type::POINT),
		m_constructed(false),
		m_strength(1.0f)
	{

	}

	void Light::create(const Vec3& position, const Vec3& color, float strength, Type type) {
		m_constructed = true;
		m_position = position;
		m_color = color;
		m_type = type;
		m_strength = strength;

	}

	bool Light::isConstructed() {
		return m_constructed;
	}

	void Light::setPosition(const Vec3& position) {
		m_position = position;
	}

	void Light::setColor(const Vec3& color) {
		m_color = color;
	}

	void Light::setStrength(double strength) {
		m_strength = (float)strength;
	}

	void Light::bind(Shader* shader, unsigned int lightNumber) {
		std::string stringPos = "light[";
		stringPos += std::to_string(lightNumber);
		stringPos += "].";
		std::string lightPos = stringPos + (std::string)"pos";
		std::string lightColor = stringPos + (std::string)"color";
		std::string lightStrength = stringPos + (std::string)"strength";
		glm::vec3 pos(m_position.x, m_position.y, m_position.z);
		glm::vec3 color(m_color.x, m_color.y, m_color.z);
		shader->setUniform(lightPos, pos);
		shader->setUniform(lightColor, color);
		shader->setUniform(lightStrength, m_strength);
		std::string lightType = stringPos + (std::string)"type";
		switch (m_type) {
			case Type::DIRECTIONAL: {
				shader->setUniform(lightType, 1);
				break;
			}
			case Type::POINT: {
				shader->setUniform(lightType, 2);
				break;
			}
		}
	}

	Light::Type Light::getType() {
		return m_type;
	}

	const Vec3& Light::getPosition() {
		return m_position;
	}

	const Vec3& Light::getColor() {
		return m_color;
	}

	const float Light::getStrength() {
		return m_strength;
	}

	Light::~Light() {

	}
}
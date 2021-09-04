#include "Light.h"

#include "Shader.h"

namespace nf {
	Light::Light() :
		m_type(Type::POINT),
		m_constructed(false),
		m_strength(1.0f)
	{

	}

	void Light::create(const Vec3& position, const Vec3& color, Type type, float strength) {
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

	void Light::bind(Shader* shader, unsigned int lightNumber) {
		glm::vec3 pos(m_position.x, m_position.y, m_position.z);
		glm::vec3 color(m_color.x, m_color.y, m_color.z);
		std::string stringPos = "light[";
		std::string stringLocation = std::to_string(lightNumber);
		stringPos += stringLocation;
		stringPos += "].";
		std::string lightPos = stringPos + (std::string)"pos";
		std::string lightColor = stringPos + (std::string)"color";
		shader->setUniform(lightPos, pos);
		shader->setUniform(lightColor, color);
		//switch (m_type) {
		//	case Type::DIRECTIONAL: {
		//		shader->setUniform(lightType.c_str(), 1);
		//		std::string lightConsant = light + (std::string)"falloffConstant";
		//		std::string lightLinear = light + (std::string)"falloffLinear";
		//		std::string lightQuad = light + (std::string)"falloffQuad";
		//		shader->setUniform(lightConsant.c_str(), 1.0f);
		//		shader->setUniform(lightLinear.c_str(), 0.7f);
		//		shader->setUniform(lightQuad.c_str(), 1.8f);
		//		break;
		//	}
		//	case Type::POINT: {
		//		//shader->setUniform(lightType.c_str(), 2);
		//		break;
		//	}
		//	case Type::SPOTLIGHT: {
		//		shader->setUniform(lightType.c_str(), 3);
		//		break;
		//	}
		//}
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
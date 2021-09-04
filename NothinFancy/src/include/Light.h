#pragma once
#include "Utility.h"

namespace nf {
	class Shader;

	class Light {
	public:
		enum class Type {
			DIRECTIONAL,
			POINT,
			SPOTLIGHT
		};
		Light();

		void create(const Vec3& position, const Vec3& color, Type type = Type::POINT, float strength = 1.0f);
		bool isConstructed();
		void setPosition(const Vec3& position);
		void setColor(const Vec3& color);

		void bind(Shader* shader, unsigned int lightNumber);

		Type getType();
		const Vec3& getPosition();
		const Vec3& getColor();
		const float getStrength();

		~Light();
	private:
		bool m_constructed;
		Type m_type;
		Vec3 m_position;
		Vec3 m_color;
		float m_strength;
	};
}
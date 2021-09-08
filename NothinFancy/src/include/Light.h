#pragma once
#include "NFObject.h"
#include "Utility.h"

namespace nf {
	class Shader;

	class Light : public NFObject {
	public:
		enum class Type {
			DIRECTIONAL,
			POINT
		};
		Light();

		void create(const Vec3& position, const Vec3& color, float strength = 1.0f, Type type = Type::POINT);
		bool isConstructed();
		void setPosition(const Vec3& position);
		void setColor(const Vec3& color);
		void setStrength(double strength);

		void bind(Shader* shader, unsigned int lightNumber);

		Type getType();
		const Vec3& getPosition();
		const Vec3& getColor();
		const float getStrength();

		void destroy() override;
		~Light();
	private:
		bool m_constructed;
		Type m_type;
		Vec3 m_position;
		Vec3 m_color;
		float m_strength;
	};
}
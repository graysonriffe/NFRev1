#pragma once
#include "NFObject.h"
#include "Utility.h"

namespace nf {
	class Shader;

	/**
	 * @brief A light in a gamestate's world
	 * 
	 * Lights are completely dynamic and can have different colors and strengths. They
	 * are also movable.
	*/
	class Light : public NFObject {
	public:
		/**
		 * @brief Dictates the behavior of a light
		*/
		enum class Type {
			/**
			 * @brief A light that has a position in the world and casts light in every
			 * direction
			*/
			POINT,
			/**
			 * @brief A light that has no position, but only a direction; Perfect for
			 * sunlight
			*/
			DIRECTIONAL
		};

		/**
		 * @brief Constructor
		 * 
		 * See @ref obLifetime
		*/
		Light();
		/**
		 * @brief Creates a light
		 * @param position Starting position vector of the light
		 * @param color Color vector in (red, green, blue) order from 0.0f to 1.0f
		 * @param strength Strength of the light; Optional
		 * @param type Type of the light; Optional
		*/
		void create(const Vec3& position, const Vec3& color, float strength = 1.0f, Type type = Type::POINT);
		/**
		 * @brief Queries whether or not the light has been created
		 * @return If the light has been created
		*/
		bool isConstructed();
		/**
		 * @brief Sets the position of the light with a Vec3
		 * @param position The position vector
		 * 
		 * For directional lights, this will change the direction instead of the position.
		*/
		void setPosition(const Vec3& position);
		/**
		 * @brief Sets the color of the light
		 * @param color The color vector in (red, green, blue) order from 0.0f to 1.0f
		*/
		void setColor(const Vec3& color);
		/**
		 * @brief Sets the strength of the light
		 * @param strength The strength
		*/
		void setStrength(float strength);
		/**
		 * @brief Queries the type of the light
		 * @return The type of the light
		*/
		Type getType();
		/**
		 * @brief Gets the position of the light
		 * @return The position of the light
		*/
		const Vec3& getPosition();
		/**
		 * @brief Gets the color of the light
		 * @return The color vector
		*/
		const Vec3& getColor();
		/**
		 * @brief Gets the strength of the light
		 * @return The strength
		*/
		const float getStrength();
#ifndef NFIMPL
		void bind(Shader* shader, unsigned int lightNumber);
#endif
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
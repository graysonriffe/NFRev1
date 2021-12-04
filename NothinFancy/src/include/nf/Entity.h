#pragma once
#include "nf/Assets.h"
#include "nf/NFObject.h"
#include "nf/Utility.h"
#ifndef NFIMPL
#include "glm/glm.hpp"
#endif

namespace nf {
	class Shader;
	class Model;

	/**
	 * @brief Represents a 3D object in the world
	 * 
	 * Every entity has a position, rotation, and scale with respect to the world.
	 * Every entity also has an associated Model it was created with.
	*/
	class Entity : public NFObject {
	public:
		/**
		 * @brief Dictates the behavior of an entity in the physics simulation
		*/
		enum class Type {
			/**
			 * @brief Will not move, but will participate in the simulation
			*/
			STATIC,
			/**
			 * @brief Will move and participate in the simulation
			*/
			DYNAMIC,
			/**
			 * @brief Will not move, but will participate in the simulation;
			 * This type allows concave meshes. This type is meant for environmental
			 * meshes such as landscapes and the insides of buildings or similar.
			*/
			MAP,
			/**
			 * @brief Will not participate in the simulation; This is perfect for debris
			 * on the ground like trash or books.
			*/
			DETAIL
		};

		Entity(const Entity& other) = delete;
		/**
		 * @brief Constructor
		 * 
		 * See @ref obLifetime
		*/
		Entity();
		/**
		 * @brief Creates an entity
		 * @param modelAsset A model Asset pointer
		 * @param position Initial position vector
		 * @param type Type of entity; Defaults to Type::STATIC
		 * 
		 * This function will initialize an entity by loading its associated model from
		 * the modelAsset parameter.
		 * 
		 * @warning Calling this function twice before the state exits will result in an
		 * error. See @ref isConstructed.
		*/
		void create(Asset* modelAsset, const Vec3& position, Type type = Type::STATIC);
		/**
		 * @brief Queries whether or not the entity has been created
		 * @return If the entity has been created
		*/
		bool isConstructed();
		/**
		 * @brief Queries the type of the entity
		 * @return The type of the entity
		*/
		Type getType();
		/**
		 * @brief Sets the position of the origin of the entity with three floats
		 * @param x X component
		 * @param y Y component
		 * @param z Z component
		*/
		void setPosition(float x, float y, float z);
		/**
		 * @brief Sets the position of the origin of the entity with a Vec3
		 * @param position The position vector
		*/
		void setPosition(const Vec3& position);
		/**
		 * @brief Sets the rotation of the origin of the entity with three floats in degrees
		 * @param x X component
		 * @param y Y component
		 * @param z Z component
		*/
		void setRotation(float x, float y, float z);
		/**
		 * @brief Sets the rotation of the origin of the entity with a Vec3
		 * @param rotation The rotation vector in degrees
		*/
		void setRotation(const Vec3& rotation);
		/**
		 * @brief Sets the scale of the entity with three floats
		 * @param x X component
		 * @param y Y component
		 * @param z Z component
		*/
		void setScale(float x, float y, float z);
		/**
		 * @brief Sets the scale of the entity with a Vec3
		 * @param scale The scale vector
		*/
		void setScale(const Vec3& scale);
		/**
		 * @brief Sets the scale of the entity with a single scalar
		 * @param x The single scalar for all three axes
		*/
		void setScale(float x);
		/**
		 * @brief Sets the linear velocy of the entity with three floats
		 * @param x X component
		 * @param y Y component
		 * @param z Z component
		*/
		void setVelocity(float x, float y, float z);
		/**
		 * @brief Sets the linear velocy of the entity with a Vec3
		 * @param velocity The velocity vector
		*/
		void setVelocity(const Vec3& velocity);
		/**
		 * @brief Sets the mass of the entity
		 * @param mass The mass
		 * 
		 * Experiment with different values to get an idea of the scale of masses.
		*/
		void setMass(float mass);
		/**
		 * @brief Queries the current position of the entity
		 * @return The entity's current position
		*/
		const Vec3& getPosition();
		/**
		 * @brief Queries the current rotation of the entity
		 * @return The entity's current rotation as a quaternion
		*/
		const Vec4& getRotation();
		/**
		 * @brief Queries the current scale of the entity
		 * @return The entity's current scale
		*/
		const Vec3& getScale();
#ifndef NFIMPL
		void setPositionPhysics(const Vec3& position);
		void setRotationPhysics(const Vec4& rotation);
		bool needsPhysicsUpdate();
		void render(Shader* shader, bool onlyDepth);
		Model* getModel() const;
		const glm::mat4 getModelMatrix();
#endif
		void destroy() override;
		~Entity();
	private:
		bool m_constructed;
		bool m_member;
		Type m_type;
		Model* m_model;

		Vec3 m_position;
		Vec4 m_rotation;
		Vec3 m_scale;

		bool m_update;
	};
}
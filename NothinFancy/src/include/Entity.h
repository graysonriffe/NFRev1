#pragma once
#include "Assets.h"
#include "NFObject.h"
#include "Utility.h"
#ifdef NFENGINE
#include "glm/glm.hpp"
#endif

namespace nf {
	class Shader;
	class Model;

	class Entity : public NFObject {
	public:
		enum class Type {
			STATIC,
			DYNAMIC,
			ENVIRONMENT,
			DETAIL
		};

		Entity();

		void create(Asset* modelAsset, Type type = Type::STATIC);
		bool isConstructed();

		Type getType();

		void setPosition(float x, float y, float z);
		void setPosition(const Vec3& position);
		void setRotation(float x, float y, float z);
		void setRotation(const Vec3& rotation);
		void setScale(float x, float y, float z);
		void setScale(const Vec3& scale);
		void setScale(float x);

		void setVelocity(float x, float y, float z);
		void setVelocity(const Vec3& velocity);

		void setMass(float mass);

		const Vec3& getPosition();
		const Vec4& getRotation();
		const Vec3& getScale();

		void setPositionPhysics(const Vec3& position);
		void setRotationPhysics(const Vec4& rotation);
		bool needsPhysicsUpdate();
		void render(Shader* shader, bool onlyDepth);
		Model* getModel() const;
#ifdef NFENGINE
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
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
			DETAIL
		};
		Entity();

		void create(Asset* modelAsset, Type type = Type::STATIC);
		bool isConstructed();

		void setType(Type type);
		Type getType();

		void setPosition(double x, double y, double z);
		void setPosition(const Vec3& position);
		void setPositionPhysics(const Vec3& position);
		void setRotation(double x, double y, double z);
		void setRotation(const Vec3& rotation);
		void setRotationPhysics(const Vec4& rotation);
		void setScale(double x);
		void setScale(double x, double y, double z);
		void setScale(const Vec3& scale);

		bool needsPhysicsUpdate();

		const Vec3& getPosition();
		const Vec4& getRotation();
		void render(Shader* shader, bool onlyDepth);
		Model* getModel() const;
#ifdef NFENGINE
		const glm::mat4 getModelMatrix();
#endif
		void destroy() override;
		~Entity();
	private:
		bool m_constructed;
		Type m_type;
		Model* m_model;

		Vec3 m_position;
		Vec4 m_rotation;
		Vec3 m_scale;

		bool m_update;
	};
}
#pragma once
#include "Assets.h"
#include "NFObject.h"
#include "Utility.h"

namespace nf {
	class Shader;
	class Model;

	class Entity : public NFObject {
	public:
		Entity();

		void create(Asset* modelAsset);
		bool isConstructed();

		void setPosition(double x, double y, double z);
		void setPosition(const Vec3& position);
		void setRotation(double x, double y, double z);
		void setRotation(const Vec3& rotation);
		void setScale(double x);
		void setScale(double x, double y, double z);
		void setScale(const Vec3& scale);

		void render(Shader* shader);
		Model* getModel() const;

		void destroy() override;
		~Entity();
	private:
		void setModelMatrix(Shader* shader);

		bool m_constructed;
		Model* m_model;

		Vec3 m_position;
		Vec3 m_rotation;
		Vec3 m_scale;
	};
}
#pragma once
#include "Model.h"
#include "Assets.h"
#include "Utility.h"

namespace nf {
	class Shader;

	class Entity {
	public:
		Entity();

		void create(Asset* modelAsset, Asset* textureAsset = nullptr);

		void setPosition(double x, double y, double z);
		void setPosition(const Vec3& position);
		void setRotation(double x, double y, double z);
		void setRotation(const Vec3& rotation);
		void setScale(double x);
		void setScale(double x, double y, double z);
		void setScale(const Vec3& scale);

		void bind(Shader* shader);
		Model* getModel() const;

		~Entity();
	private:
		void setModelMatrix(Shader* shader);

		Model* m_model;

		Vec3 m_position;
		Vec3 m_rotation;
		Vec3 m_scale;
	};
}
#pragma once
#include "Model.h"
#include "Assets.h"

namespace nf {
	class Entity {
		struct Vec3 {
			Vec3(double x1) : x(x1), y(x1), z(x1) {}
			Vec3(double x1, double y1, double z1) : x(x1), y(y1), z(z1) {}
			double x, y, z;
		};
	public:
		Entity();

		void create(Asset* modelAsset, Asset* textureAsset = nullptr);

		void setPosition(double x, double y, double z);
		void setRotation(double x, double y, double z);
		void setScale(double x);
		void setScale(double x, double y, double z);

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
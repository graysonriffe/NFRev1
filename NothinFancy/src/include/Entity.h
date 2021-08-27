#pragma once
#include "Model.h"

namespace nf {
	class Entity {
		struct Vec3 {
			Vec3(float x1) : x(x1), y(x1), z(x1) {}
			Vec3(float x1, float y1, float z1) : x(x1), y(y1), z(z1) {}
			float x, y, z;
		};
	public:
		Entity();

		void create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount);
		//TODO: Do this using loaded assets somehow
		void setLocation(float x, float y, float z);
		void setRotation(float x, float y, float z);
		void setScale(float x);
		void setScale(float x, float y, float z);

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
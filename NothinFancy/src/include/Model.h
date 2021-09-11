#pragma once
#include "Drawable.h"
#include "Assets.h"
#include "Utility.h"

namespace nf {
	class Drawable;
	class Texture;
	class Shader;

	class Material : public Drawable {
	public:
		Material(const void* vb, const size_t vbSize, const void* tc, const size_t tcSize, const void* vn, const size_t vnSize, const void* ib, const unsigned int ibCount, ATexture* diffTex, Vec3& diffColor, ATexture* specTex, float shininess);

		void render(Shader* shader);

		~Material();
	private:
		bool m_hasDiffuse = false;
		Texture* m_diffuseTexture = nullptr;
		Vec3 m_diffColor;
		bool m_hasSpecular = false;
		Texture* m_specularTexture = nullptr;
		float m_shininess;
	};

	class Model {
	public:
		Model(AModel* model);

		void render(Shader* shader);

		bool isBaseAsset();

		~Model();
	private:
		bool m_base;
		std::vector<Material*> m_materials;
	};
}
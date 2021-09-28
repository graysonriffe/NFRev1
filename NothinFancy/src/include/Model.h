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
		Material(const void* vb, const size_t vbSize, const void* tc, const size_t tcSize, const void* vn, const size_t vnSize, const void* tan, const size_t tanSize, const void* ib, const unsigned int ibCount, ATexture* diffTex, Vec3& diffColor, ATexture* specTex, float shininess, ATexture* normalTex);

		void render(Shader* shader, bool onlyDepth);

		~Material();
	private:
		bool m_hasDiffuse = false;
		Texture* m_diffuseTexture = nullptr;
		Vec3 m_diffColor;
		bool m_hasSpecular = false;
		Texture* m_specularTexture = nullptr;
		float m_shininess;
		bool m_hasNormal = false;
		Texture* m_normalTexture = nullptr;
	};

	class Model {
	public:
		Model(AModel* model);

		void render(Shader* shader, bool onlyDepth);

		bool isBaseAsset();

		~Model();
	private:
		bool m_base;
		std::vector<Material*> m_materials;
	};
}
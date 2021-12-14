#pragma once
#include "nf/Drawable.h"
#include "nf/Assets.h"
#include "nf/Utility.h"

namespace nf {
	class Texture;
	class Shader;
	struct TempMaterial;

	class Model : public Drawable {
	public:
		Model(AModel* model, bool physicsConvex, bool physicsTriangle);

		void render(Shader* shader, bool onlyDepth, unsigned int count);
		void bindMaterials(Shader* shader);

		bool isBaseAsset();

		~Model();
	private:
		struct Material {
			Texture* diffuse;
			Texture* specular;
			Texture* normal;
			Vec3 diffuseColor;
			float shininess = 1.0f;
		};

		void createMaterials(const std::string& mats, AModel* model);

		bool m_base;
		std::vector<Material> m_materials;
		const std::string m_hasDiffString = "hasDiffuseTex[";
		const std::string m_diffString = "diffuseTexture[";
		const std::string m_diffColorString = "diffuseColor[";
		const std::string m_hasSpecString = "hasSpecTex[";
		const std::string m_specString = "specularTexture[";
		const std::string m_hasNormString = "hasNormTex[";
		const std::string m_normString = "normalTexture[";
		const std::string m_specPowerString = "specPower[";
	};
}
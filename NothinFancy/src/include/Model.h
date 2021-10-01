#pragma once
#include "Drawable.h"
#include "Assets.h"
#include "Utility.h"

namespace nf {
	class Texture;
	class Shader;

	class Model : public Drawable {
	public:
		Model(AModel* model);

		void render(Shader* shader, bool onlyDepth, unsigned int count);
		void bindMaterials(Shader* shader);

		bool isBaseAsset();

		~Model();
	private:
		bool m_base;
		std::vector<std::tuple<Texture*, Texture*, Texture*, float, float, float, float>> m_materials;
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
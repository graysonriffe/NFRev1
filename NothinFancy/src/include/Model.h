#pragma once
#include "Drawable.h"
#include "Assets.h"
#include "Utility.h"

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
		struct TempMaterial {
			std::vector<float> outVB;
			std::vector<float> unindexedVB;
			std::vector<unsigned int> vbIndices;
			std::vector<float> outTC;
			std::vector<float> unindexedTC;
			std::vector<unsigned int> tcIndices;
			std::vector<float> outVN;
			std::vector<float> unindexedVN;
			std::vector<unsigned int> vnIndices;
			std::vector<float> unindexedTan;
			std::vector<float> outTan;
			std::vector<unsigned int> outIB;
			unsigned int ibCount = 0;

			std::string diffuseTextureName;
			Vec3 diffuseColor;
			std::string specularTextureName;
			std::string normalTextureName;
			float shininess = 1.0f;
		};

		void parseMaterials(std::unordered_map<std::string, TempMaterial*>& mats, std::vector<char>& mtl);

		bool m_base;
		std::vector<std::tuple<Texture*, Texture*, Texture*, float, float, float, float>> m_materials;
		const char* m_newMtl;
		const char* m_newLine;
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
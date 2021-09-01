#pragma once
#include <unordered_map>

namespace nf {
	class Model;

	struct Asset {
		char* data;

		virtual ~Asset();
	};

	struct AModel : Asset {
		bool alreadyLoaded;
		Model* loadedModel;

		~AModel() override;
	};

	struct ATexture : Asset {
		size_t size;

		~ATexture() override;
	};

	struct AShader : Asset {
		~AShader() override;
	};

	class AssetPack {
	public:
		AssetPack();

		void load(const char* packName);
		Asset* operator[](const char* in);
		Asset* operator[](std::string& in);

		~AssetPack();
	private:
		std::unordered_map<std::string, Asset*> m_assets;
	};

	struct BaseAssets {
		static AModel* cube;
		static AModel* plane;
		static AModel* sphere;
		static AModel* cone;
		static AModel* cylinder;
		static AModel* torus;

		static ATexture* logo;
	};
}
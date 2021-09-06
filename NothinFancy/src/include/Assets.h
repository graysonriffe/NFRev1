#pragma once
#include <unordered_map>

namespace nf {
	class Model;
	class Texture;
	struct Font;

	struct Asset {
		char* data;
		bool alreadyLoaded = false;
		virtual ~Asset();
	};

	struct AModel : Asset {
		Model* loadedModel;

		~AModel() override;
	};

	struct ATexture : Asset {
		size_t size;
		Texture* loadedTexture;

		~ATexture() override;
	};

	struct ACubemap : Asset {
		char* frontData;
		size_t frontSize;
		char* backData;
		size_t backSize;
		char* topData;
		size_t topSize;
		char* bottomData;
		size_t bottomSize;
		char* leftData;
		size_t leftSize;
		char* rightData;
		size_t rightSize;

		unsigned int numImages;

		~ACubemap();
	};

	struct AShader : Asset {
		~AShader() override;
	};

	struct AFont : Asset {
		size_t size;
		Font* loadedFont;

		~AFont() override;
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

		static AFont* defaultFont;

		//static ATexture* logo;
	};
}
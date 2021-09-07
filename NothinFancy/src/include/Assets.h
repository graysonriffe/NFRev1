#pragma once
#include <unordered_map>

namespace nf {
	class Model;
	class Texture;
	struct Font;

	struct Asset {
		char* data = nullptr;
		bool alreadyLoaded = false;
		virtual ~Asset();
	};

	struct AModel : Asset {
		Model* loadedModel = nullptr;

		~AModel() override;
	};

	struct ATexture : Asset {
		size_t size = 0;
		Texture* loadedTexture = nullptr;

		~ATexture() override;
	};

	struct ACubemap : Asset {
		char* frontData = nullptr;
		size_t frontSize = 0;
		char* backData = nullptr;
		size_t backSize = 0;
		char* topData = nullptr;
		size_t topSize = 0;
		char* bottomData = nullptr;
		size_t bottomSize = 0;
		char* leftData = nullptr;
		size_t leftSize = 0;
		char* rightData = nullptr;
		size_t rightSize = 0;

		unsigned int numImages = 0;

		~ACubemap();
	};

	struct AShader : Asset {
		~AShader() override;
	};

	struct AFont : Asset {
		size_t size = 0;
		Font* loadedFont = nullptr;

		~AFont() override;
	};

	class AssetPack {
	public:
		AssetPack();

		void load(const char* packName);
		Asset* operator[](const char* in);
		Asset* operator[](std::string& in);

		void unload();
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

		static AFont* defaultFont;

	};
}
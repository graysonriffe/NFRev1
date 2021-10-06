#pragma once
#include <unordered_map>

#include "NFObject.h"

namespace nf {
	class Model;
	class Texture;
	struct Font;

	struct Asset {
		char* data = nullptr;
		size_t size = 0;
		bool alreadyLoaded = false;
		bool isBaseAsset = false;
		virtual ~Asset();
	};

	struct ATexture : Asset {
		Texture* loadedTexture = nullptr;
		~ATexture() override;
	};

	struct AModel : Asset {
		Model* loadedModel = nullptr;

		std::unordered_map<std::string, ATexture*> neededTextures;

		~AModel() override;
	};

	struct ACubemap : Asset {
		char* frontData = nullptr;
		unsigned int frontSize = 0;
		char* backData = nullptr;
		unsigned int backSize = 0;
		char* topData = nullptr;
		unsigned int topSize = 0;
		char* bottomData = nullptr;
		unsigned int bottomSize = 0;
		char* leftData = nullptr;
		unsigned int leftSize = 0;
		char* rightData = nullptr;
		unsigned int rightSize = 0;

		unsigned int numImages = 0;

		~ACubemap();
	};

	struct AFont : Asset {
		size_t size = 0;
		Font* loadedFont = nullptr;

		~AFont() override;
	};

	struct AButton : Asset {
		ATexture idleTex;
		ATexture hoverTex;
		ATexture pressedTex;

		unsigned int numImages = 0;

		~AButton() override;
	};

	struct AShader : Asset {
		~AShader() override;
	};

	struct ASound : Asset {
		~ASound() override;
	};

	class AssetPack : public NFObject {
	public:
		AssetPack();

		void load(const char* packName);
		Asset* operator[](const char* in);
		Asset* operator[](std::string& in);

		void destroy() override;
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

		static ACubemap* cubemap;

		static AFont* font;

		static AButton* button;
	};
}
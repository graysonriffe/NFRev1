#pragma once
#include <unordered_map>

namespace nf {
	class Model;

	struct Asset {
		virtual ~Asset();
	};

	struct AModel : Asset {
		char* data;
		bool alreadyLoaded;
		Model* loadedModel;

		~AModel() override;
	};

	struct ATexture : Asset {
		unsigned char* data;
		size_t size;

		~ATexture() override;
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
}
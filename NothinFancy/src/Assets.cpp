#include "Assets.h"

#include "Utility.h"

namespace nf {
	Asset::~Asset() {

	}

	AGeometry::~AGeometry() {
		delete[] m_vertexBufferData;
		delete[] m_indexBufferData;
		delete[] m_textureCoordinatesBufferData;
	}

	ATexture::~ATexture() {
		delete[] m_data;
	}

	AssetPack::AssetPack() {

	}

	void AssetPack::load(const char* packName) {
		std::string path = "assets/" + (std::string)packName + ".nfpack";
		std::string contents = readFile(path);
	}

	AssetPack::~AssetPack() {
		for (auto curr : m_assets) {
			delete curr.second;
		}
	}
}
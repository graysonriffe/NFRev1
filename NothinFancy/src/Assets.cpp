#include "Assets.h"

#include "Model.h"
#include "Utility.h"

namespace nf {
	Asset::~Asset() {

	}

	AModel::~AModel() {
		delete[] data;
	}

	ATexture::~ATexture() {
		delete[] data;
	}

	AssetPack::AssetPack() {

	}

	void AssetPack::load(const char* packName) {
		std::string path = "assets/" + (std::string)packName;
		std::string packContents = readFile(path);
		while (packContents.size()) {
			unsigned int startingPos = packContents.find_first_of("#NFASSET ") + 9;
			packContents = packContents.substr(9);
			unsigned int endAssetNamePos = packContents.find_first_of('\n');
			std::string assetName = packContents.substr(0, endAssetNamePos);
			packContents = packContents.substr(endAssetNamePos + 1);
			unsigned int extensionPos = assetName.find_first_of('.');
			std::string extension = assetName.substr(extensionPos + 1);
			std::string assetContents;
			unsigned int nextAssetPos = packContents.find("#NFASSET ");
			if (nextAssetPos != std::string::npos) {
				assetContents = packContents.substr(0, nextAssetPos - 1);
				packContents = packContents.substr(nextAssetPos);
			}
			else {
				assetContents = packContents;
				packContents = "";
			}
			size_t assetSize = assetContents.size();

			if (extension == "obj") {
				AModel* geometry = new AModel;
				geometry->data = new char[assetSize + 1];
				std::memcpy(geometry->data, &assetContents[0], assetSize);
				geometry->data[assetSize] = '\0';
				geometry->alreadyLoaded = false;
				geometry->loadedModel = nullptr;
				m_assets[assetName] = geometry;
				continue;
			}
			if (extension == "png") {
				ATexture* texture = new ATexture;
				texture->data = new unsigned char[assetSize];
				std::memcpy(texture->data, &assetContents[0], assetSize);
				texture->size = assetSize;
				m_assets[assetName] = texture;
				continue;
			}
			Error("Invalid asset extention in pack \"" + (std::string)packName + (std::string)"\"!");
		}
	}

	Asset* AssetPack::operator[](const char* in) {
		if (m_assets.find(in) == m_assets.end())
			Error("Could not find asset \"" + (std::string)in + (std::string)"\" in asset pack!");
		return m_assets[in];
	}
	Asset* AssetPack::operator[](std::string& in) {
		if (m_assets.find(in) == m_assets.end())
			Error("Could not find asset \"" + in + (std::string)"\" in asset pack!");
		return m_assets[in];
	}

	AssetPack::~AssetPack() {
		for (auto curr : m_assets) {
			delete curr.second;
		}
	}
}
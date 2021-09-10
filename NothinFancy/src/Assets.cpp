#include "Assets.h"

#include <sstream>

#include "Application.h"
#include "Model.h"
#include "Utility.h"

namespace nf {
	Asset::~Asset() {
		delete[] data;
	}

	ATexture::~ATexture() {

	}

	AModel::~AModel() {

	}

	ACubemap::~ACubemap() {
		delete[] frontData;
		delete[] backData;
		delete[] topData;
		delete[] bottomData;
		delete[] leftData;
		delete[] rightData;
	}

	AShader::~AShader() {

	}

	AFont::~AFont() {

	}

	AssetPack::AssetPack() {

	}

	void AssetPack::load(const char* packName) {
		std::string path = "assets/" + (std::string)packName;
		std::string packContents = readFile(path);
		std::string packContentsOBJ = packContents;
		std::unordered_map<std::string, ACubemap*> cubemaps;
		unsigned int cubemapCount = 0;
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

			if (extension == "obj")
				continue;
			if (extension == "png") {
				if (assetName.find("_cmfront") != std::string::npos || assetName.find("_cmback") != std::string::npos || assetName.find("_cmtop") != std::string::npos || assetName.find("_cmbottom") != std::string::npos || assetName.find("_cmleft") != std::string::npos || assetName.find("_cmright") != std::string::npos) {
					std::string cmName = assetName.substr(0, assetName.find('_'));
					ACubemap* curr;
					if (cubemaps.find(cmName) == cubemaps.end()) {
						cubemaps[cmName] = new ACubemap;
						cubemaps[cmName]->numImages = 0;
					}
					curr = cubemaps[cmName];
					if (curr->numImages == 6)
						Error("Duplicate cubemap images in pack \"" + (std::string)packName + (std::string)"\"!");
					if (assetName.find("_cmfront") != std::string::npos) {
						curr->frontData = new char[assetSize];
						std::memcpy(curr->frontData, &assetContents[0], assetSize);
						curr->frontSize = assetSize;
					}
					if (assetName.find("_cmback") != std::string::npos) {
						curr->backData = new char[assetSize];
						std::memcpy(curr->backData, &assetContents[0], assetSize);
						curr->backSize = assetSize;
					}
					if (assetName.find("_cmtop") != std::string::npos) {
						curr->topData = new char[assetSize];
						std::memcpy(curr->topData, &assetContents[0], assetSize);
						curr->topSize = assetSize;
					}
					if (assetName.find("_cmbottom") != std::string::npos) {
						curr->bottomData = new char[assetSize];
						std::memcpy(curr->bottomData, &assetContents[0], assetSize);
						curr->bottomSize = assetSize;
					}
					if (assetName.find("_cmleft") != std::string::npos) {
						curr->leftData = new char[assetSize];
						std::memcpy(curr->leftData, &assetContents[0], assetSize);
						curr->leftSize = assetSize;
					}
					if (assetName.find("_cmright") != std::string::npos) {
						curr->rightData = new char[assetSize];
						std::memcpy(curr->rightData, &assetContents[0], assetSize);
						curr->rightSize = assetSize;
					}
					curr->numImages++;

					if (curr->numImages == 6) {
						m_assets[cmName + (std::string)".cm"] = curr;
					}

					cubemapCount++;
					continue;
				}
				ATexture* texture = new ATexture;
				texture->data = new char[assetSize];
				std::memcpy(texture->data, &assetContents[0], assetSize);
				texture->size = assetSize;
				if (packName == "base.nfpack")
					texture->isBaseAsset = true;
				m_assets[assetName] = texture;
				continue;
			}
			if (extension == "shader") {
				AShader* shader = new AShader;
				shader->data = new char[assetSize + 1];
				std::memcpy(shader->data, &assetContents[0], assetSize);
				shader->data[assetSize] = '\0';
				m_assets[assetName] = shader;
				continue;
			}
			if (extension == "ttf") {
				AFont* font = new AFont;
				font->data = new char[assetSize];
				std::memcpy(font->data, &assetContents[0], assetSize);
				font->size = assetSize;
				if (packName == "base.nfpack")
					font->isBaseAsset = true;
				m_assets[assetName] = font;
				continue;
			}
			Error("Invalid asset extention in pack \"" + (std::string)packName + (std::string)"\"!");
		}
		if (cubemapCount % 6 != 0)
			Error("Could not find full cubemap in pack \"" + (std::string)packName + (std::string)"\"!");

		while (packContentsOBJ.size()) {
			unsigned int startingPos = packContentsOBJ.find_first_of("#NFASSET ") + 9;
			packContentsOBJ = packContentsOBJ.substr(9);
			unsigned int endAssetNamePos = packContentsOBJ.find_first_of('\n');
			std::string assetName = packContentsOBJ.substr(0, endAssetNamePos);
			packContentsOBJ = packContentsOBJ.substr(endAssetNamePos + 1);
			unsigned int extensionPos = assetName.find_first_of('.');
			std::string extension = assetName.substr(extensionPos + 1);
			std::string assetContents;
			unsigned int nextAssetPos = packContentsOBJ.find("#NFASSET ");
			if (nextAssetPos != std::string::npos) {
				assetContents = packContentsOBJ.substr(0, nextAssetPos - 1);
				packContentsOBJ = packContentsOBJ.substr(nextAssetPos);
			}
			else {
				assetContents = packContentsOBJ;
				packContentsOBJ = "";
			}
			size_t assetSize = assetContents.size();

			if (extension == "obj") {
				AModel* model = new AModel;
				std::string textures = assetContents.substr(0, assetContents.find("\n"));
				if (textures != "none") {
					std::stringstream ss(textures);
					std::string curr;
					while (ss >> curr) {
						model->neededTextures[curr] = (ATexture*)m_assets[curr];
					}
				}
				assetContents = assetContents.substr(assetContents.find("\n") + 1);
				model->data = new char[assetSize + 1];
				std::memcpy(model->data, &assetContents[0], assetSize);
				model->data[assetSize] = '\0';
				if (packName == "base.nfpack")
					model->isBaseAsset = true;
				m_assets[assetName] = model;
				continue;
			}
		}

		if (packName != "base.nfpack")
			Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
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

	void AssetPack::destroy() {
		unload();
	}

	void AssetPack::unload() {
		for (auto curr : m_assets)
			delete curr.second;
		m_assets.clear();
	}

	AssetPack::~AssetPack() {
		unload();
	}

	AModel* BaseAssets::cube;
	AModel* BaseAssets::plane;
	AModel* BaseAssets::sphere;
	AModel* BaseAssets::cone;
	AModel* BaseAssets::cylinder;
	AModel* BaseAssets::torus;

	ATexture* BaseAssets::logo;

	AFont* BaseAssets::defaultFont;
}
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

	AFont::~AFont() {

	}

	AButton::~AButton() {

	}

	AShader::~AShader() {

	}

	ASound::~ASound() {

	}

	AssetPack::AssetPack() :
		m_loaded(false)
	{

	}

	void AssetPack::load(const char* packName) {
		std::string path = "assets/" + (std::string)packName;
		std::string packContents = readFile(path, true);
		std::string packContentsOBJ = packContents;
		std::unordered_map<std::string, ACubemap*> cubemaps;
		std::unordered_map<std::string, AButton*> buttons;
		unsigned int cubemapCount = 0;
		unsigned int buttonCount = 0;
		while (packContents.size()) {
			size_t startingPos = packContents.find_first_of("#NFASSET ") + 9;
			packContents = packContents.substr(9);
			size_t endAssetNamePos = packContents.find_first_of('\n');
			std::string assetName = packContents.substr(0, endAssetNamePos);
			packContents = packContents.substr(endAssetNamePos + 1);
			size_t extensionPos = assetName.find_first_of('.');
			std::string extension = assetName.substr(extensionPos + 1);
			std::string assetContents;
			size_t nextAssetPos = packContents.find("#NFASSET ");
			if (nextAssetPos != std::string::npos) {
				assetContents = packContents.substr(0, nextAssetPos - 1);
				packContents = packContents.substr(nextAssetPos);
			}
			else {
				assetContents = packContents;
				packContents = "";
			}
			unsigned int assetSize = (unsigned int)assetContents.size();

			if (extension == "obj")
				continue;
			if (extension == "png" || extension == "jpg") {
				if (assetName.find("_cmfront") != std::string::npos || assetName.find("_cmback") != std::string::npos || assetName.find("_cmtop") != std::string::npos || assetName.find("_cmbottom") != std::string::npos || assetName.find("_cmleft") != std::string::npos || assetName.find("_cmright") != std::string::npos) {
					std::string cmName = assetName.substr(0, assetName.find('_'));
					ACubemap* curr;
					if (cubemaps.find(cmName) == cubemaps.end()) {
						cubemaps[cmName] = new ACubemap;
					}
					curr = cubemaps[cmName];
					if (curr->numImages == 6)
						NFError("Duplicate cubemap images in pack \"" + (std::string)packName + (std::string)"\"!");
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

					if (curr->numImages == 6)
						m_assets[cmName + (std::string)".cm"] = curr;

					cubemapCount++;
					continue;
				}
				else if (assetName.find("_buttonidle") != std::string::npos || assetName.find("_buttonhover") != std::string::npos || assetName.find("_buttonpressed") != std::string::npos) {
					std::string buttonName = assetName.substr(0, assetName.find('_'));
					AButton* curr;
					if (buttons.find(buttonName) == buttons.end()) {
						buttons[buttonName] = new AButton;
						buttons[buttonName]->numImages = 0;
					}
					curr = buttons[buttonName];
					if (curr->numImages == 3)
						NFError("Duplicate button images in pack \"" + (std::string)packName + (std::string)"\"!");
					if (assetName.find("_buttonidle") != std::string::npos) {
						curr->idleTex.data = new char[assetSize];
						std::memcpy(curr->idleTex.data, &assetContents[0], assetSize);
						curr->idleTex.size = assetSize;
					}
					if (assetName.find("_buttonhover") != std::string::npos) {
						curr->hoverTex.data = new char[assetSize];
						std::memcpy(curr->hoverTex.data, &assetContents[0], assetSize);
						curr->hoverTex.size = assetSize;
					}
					if (assetName.find("_buttonpressed") != std::string::npos) {
						curr->pressedTex.data = new char[assetSize];
						std::memcpy(curr->pressedTex.data, &assetContents[0], assetSize);
						curr->pressedTex.size = assetSize;
					}
					curr->numImages++;

					if (curr->numImages == 3)
						m_assets[buttonName + (std::string)".button"] = curr;

					buttonCount++;
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
			if (extension == "ogg" || extension == "wav") {
				ASound* sound = new ASound;
				sound->data = new char[assetSize];
				std::memcpy(sound->data, &assetContents[0], assetSize);
				sound->size = assetSize;
				if (packName == "base.nfpack")
					sound->isBaseAsset = true;
				m_assets[assetName] = sound;
				continue;
			}
			NFError("Invalid asset extention in pack \"" + (std::string)packName + (std::string)"\"!");
		}
		if (cubemapCount % 6 != 0)
			NFError("Could not find full cubemap in pack \"" + (std::string)packName + (std::string)"\"!");
		if (buttonCount % 3 != 0)
			NFError("Could not find full button set in pack \"" + (std::string)packName + (std::string)"\"!");

		while (packContentsOBJ.size()) {
			size_t startingPos = packContentsOBJ.find_first_of("#NFASSET ") + 9;
			packContentsOBJ = packContentsOBJ.substr(9);
			size_t endAssetNamePos = packContentsOBJ.find_first_of('\n');
			std::string assetName = packContentsOBJ.substr(0, endAssetNamePos);
			packContentsOBJ = packContentsOBJ.substr(endAssetNamePos + 1);
			size_t extensionPos = assetName.find_first_of('.');
			std::string extension = assetName.substr(extensionPos + 1);
			std::string assetContents;
			size_t nextAssetPos = packContentsOBJ.find("#NFASSET ");
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
				model->data = new char[assetSize];
				std::memcpy(model->data, &assetContents[0], assetSize);
				model->size = assetSize;
				if (packName == "base.nfpack")
					model->isBaseAsset = true;
				m_assets[assetName] = model;
				continue;
			}
		}

		m_loaded = true;
		if (packName != "base.nfpack") {
			if (!Application::getApp()->getCurrentState()->isRunning())
				Application::getApp()->getCurrentState()->m_nfObjects.push_back(this);
		}
	}

	bool AssetPack::isLoaded() {
		return m_loaded;
	}

	Asset* AssetPack::get(const char* in) {
		if (!m_loaded) NFError("AssetPack has not been loaded yet!");
		if (m_assets.find(in) == m_assets.end())
			NFError("Could not find asset \"" + (std::string)in + (std::string)"\" in asset pack!");
		return m_assets[in];
	}

	Asset* AssetPack::get(std::string& in) {
		if (!m_loaded) NFError("AssetPack has not been loaded yet!");
		return get(in.c_str());
	}

	Asset* AssetPack::operator[](const char* in) {
		if (!m_loaded) NFError("AssetPack has not been loaded yet!");
		return get(in);
	}
	Asset* AssetPack::operator[](std::string& in) {
		if (!m_loaded) NFError("AssetPack has not been loaded yet!");
		return get(in.c_str());
	}

	void AssetPack::destroy() {
		unload();
	}

	void AssetPack::unload() {
		for (auto curr : m_assets)
			delete curr.second;
		m_assets.clear();

		m_loaded = false;
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

	ACubemap* BaseAssets::cubemap;

	AFont* BaseAssets::font;

	AButton* BaseAssets::button;
}
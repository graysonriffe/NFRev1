#include "nf/Assets.h"

#include <sstream>

#include "nf/Application.h"
#include "nf/Model.h"
#include "nf/Utility.h"

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
		NFLog("Loading asset pack \"" + (std::string)packName + (std::string)"\"");
		std::string path = "assets/" + (std::string)packName;
		std::string packContents;
		if (!readFile(path, packContents, true))
			NFError("Could not find pack file \"" + (std::string)packName + (std::string)"\"!") ;
		std::string packContentsOBJ = packContents;
		std::unordered_map<std::string, ACubemap*> cubemaps;
		std::unordered_map<std::string, AButton*> buttons;
		unsigned int cubemapFaceCount = 0;
		unsigned int buttonTextureCount = 0;

		unsigned int assetCount = std::stoi(packContents.substr(0, packContents.find_first_of("\n")));

		unsigned int endHeader = (unsigned int)packContents.find("/ENDHEADER");
		unsigned int headerLength = endHeader - ((unsigned int)packContents.find_first_of("\n") + 1);
		std::stringstream header(packContents.substr(packContents.find_first_of("\n") + 1, headerLength));
		unsigned int assetPos = endHeader + 10;
		for (unsigned int i = 0; i < assetCount; i++) {
			std::string assetName;
			std::getline(header, assetName);
			std::string temp;
			std::getline(header, temp);
			unsigned int assetSize = std::stoi(temp);

			std::string extension = assetName.substr(assetName.find_last_of(".") + 1);

			if (extension == "obj") {
				assetPos += assetSize;
				continue;
			}

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
						std::memcpy(curr->frontData, &packContents[assetPos], assetSize);
						curr->frontSize = assetSize;
					}
					if (assetName.find("_cmback") != std::string::npos) {
						curr->backData = new char[assetSize];
						std::memcpy(curr->backData, &packContents[assetPos], assetSize);
						curr->backSize = assetSize;
					}
					if (assetName.find("_cmtop") != std::string::npos) {
						curr->topData = new char[assetSize];
						std::memcpy(curr->topData, &packContents[assetPos], assetSize);
						curr->topSize = assetSize;
					}
					if (assetName.find("_cmbottom") != std::string::npos) {
						curr->bottomData = new char[assetSize];
						std::memcpy(curr->bottomData, &packContents[assetPos], assetSize);
						curr->bottomSize = assetSize;
					}
					if (assetName.find("_cmleft") != std::string::npos) {
						curr->leftData = new char[assetSize];
						std::memcpy(curr->leftData, &packContents[assetPos], assetSize);
						curr->leftSize = assetSize;
					}
					if (assetName.find("_cmright") != std::string::npos) {
						curr->rightData = new char[assetSize];
						std::memcpy(curr->rightData, &packContents[assetPos], assetSize);
						curr->rightSize = assetSize;
					}
					assetPos += assetSize;
					curr->numImages++;

					if (curr->numImages == 6)
						m_assets[cmName + (std::string)".cm"] = curr;

					cubemapFaceCount++;
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
						std::memcpy(curr->idleTex.data, &packContents[assetPos], assetSize);
						curr->idleTex.size = assetSize;
					}
					if (assetName.find("_buttonhover") != std::string::npos) {
						curr->hoverTex.data = new char[assetSize];
						std::memcpy(curr->hoverTex.data, &packContents[assetPos], assetSize);
						curr->hoverTex.size = assetSize;
					}
					if (assetName.find("_buttonpressed") != std::string::npos) {
						curr->pressedTex.data = new char[assetSize];
						std::memcpy(curr->pressedTex.data, &packContents[assetPos], assetSize);
						curr->pressedTex.size = assetSize;
					}
					assetPos += assetSize;
					curr->numImages++;

					if (curr->numImages == 3)
						m_assets[buttonName + (std::string)".button"] = curr;

					buttonTextureCount++;
					continue;
				}
				ATexture* texture = new ATexture;
				texture->data = new char[assetSize];
				std::memcpy(texture->data, &packContents[assetPos], assetSize);
				assetPos += assetSize;
				texture->size = assetSize;
				if (packName == "base.nfpack")
					texture->isBaseAsset = true;
				m_assets[assetName] = texture;
				continue;
			}
			if (extension == "shader") {
				AShader* shader = new AShader;
				shader->data = new char[assetSize + 1];
				std::memcpy(shader->data, &packContents[assetPos], assetSize);
				assetPos += assetSize;
				shader->data[assetSize] = '\0';
				m_assets[assetName] = shader;
				continue;
			}
			if (extension == "ttf") {
				AFont* font = new AFont;
				font->data = new char[assetSize];
				std::memcpy(font->data, &packContents[assetPos], assetSize);
				assetPos += assetSize;
				font->size = assetSize;
				if (packName == "base.nfpack")
					font->isBaseAsset = true;
				m_assets[assetName] = font;
				continue;
			}
			if (extension == "ogg" || extension == "wav") {
				ASound* sound = new ASound;
				sound->data = new char[assetSize];
				std::memcpy(sound->data, &packContents[assetPos], assetSize);
				assetPos += assetSize;
				sound->size = assetSize;
				if (packName == "base.nfpack")
					sound->isBaseAsset = true;
				m_assets[assetName] = sound;
				continue;
			}
			NFError("Invalid asset extention in pack \"" + (std::string)packName + (std::string)"\"!");
		}

		if (cubemapFaceCount % 6 != 0)
			NFError("Could not find full cubemap in pack \"" + (std::string)packName + (std::string)"\"!");
		if (buttonTextureCount % 3 != 0)
			NFError("Could not find full button set in pack \"" + (std::string)packName + (std::string)"\"!");

		//Model pass after ATextures are loaded

		assetPos = endHeader + 10;
		std::stringstream header2(packContents.substr(packContents.find_first_of("\n") + 1, headerLength));
		for (unsigned int i = 0; i < assetCount; i++) {
			std::string assetName;
			std::getline(header2, assetName);
			std::string temp;
			std::getline(header2, temp);
			unsigned int assetSize = std::stoi(temp);

			std::string extension = assetName.substr(assetName.find_last_of(".") + 1);

			if (extension == "obj") {
				AModel* model = new AModel;
				std::string textures = packContents.substr(assetPos, packContents.find("/ENDTEXTURES", assetPos) - assetPos - 1);
				if (textures != "none") {
					std::stringstream ss(textures);
					std::string curr;
					while (std::getline(ss, curr))
						model->neededTextures.push_back((ATexture*)m_assets[curr]);
				}

				unsigned int newStartPos = (unsigned int)packContents.find("/ENDTEXTURES", assetPos) + 13;
				assetSize -= newStartPos - assetPos;
				assetPos = newStartPos;
				model->data = new char[assetSize];
				std::memcpy(model->data, &packContents[assetPos], assetSize);
				assetPos += assetSize;
				model->size = assetSize;
				if (packName == "base.nfpack")
					model->isBaseAsset = true;
				m_assets[assetName] = model;
				continue;

			}

			assetPos += assetSize;
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
#pragma once
#include <unordered_map>

#include "nf/NFObject.h"

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
		std::vector<ATexture*> neededTextures;

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

	/**
	 * @brief A representation of a nfpack file
	 * 
	 * With this class, you can access your custom assets with the get functions.
	*/
	class AssetPack : public NFObject {
	public:
		AssetPack(const AssetPack& other) = delete;
		/**
		 * @brief Constructor
		 * 
		 * See @ref obLifetime
		*/
		AssetPack();
		/**
		 * @brief Loads a specified nfpack into memory
		 * @param packName name of pack to load relative to the `assets` directory
		 * 
		 * After calling this function, future calls to @ref get will return an Asset pointer.
		 * 
		 * @warning Because this function could take a considerable amount of time when
		 * dealing with very large packs, it is not recomended to call this function
		 * in a Gamestate::update or Gamestate::render function.
		*/
		void load(const char* packName);
		/**
		 * @brief Queries whether or not a pack has been loaded
		 * @return If a pack is loaded
		*/
		bool isLoaded();
		/**
		 * @brief Gets a specified Asset pointer from a string literal
		 * @param in name of asset file to retrieve
		 * @return An Asset pointer
		*/
		Asset* get(const char* in);
		/**
		 * @brief Gets a specified Asset pointer from an std::string
		 * @param in name of asset file to retrieve
		 * @return An Asset pointer
		*/
		Asset* get(std::string& in);
		/**
		 * @brief Gets a specified Asset pointer from a string literal
		 * @param in name of asset file to retrieve
		 * @return An Asset pointer
		 * 
		 * This function allows you to get assets like this:
		 * 
		 * ~~~
		 * Asset* model = assetPack["model.obj"];
		 * ~~~
		*/
		Asset* operator[](const char* in);
		/**
		 * @brief Gets a specified Asset pointer from an std::string
		 * @param in name of asset file to retrieve
		 * @return An Asset pointer
		 *
		 * This function allows you to get assets like this:
		 *
		 * ~~~
		 * std::string str = "model.obj";
		 * Asset* model = assetPack[str];
		 * ~~~
		*/
		Asset* operator[](std::string& in);
		/**
		 * @brief Unloads the pack from memory
		 * 
		 * This function can be useful if you have an AssetPack object as a member of
		 * a Gamestate and want to unload it at a specific time.
		*/
		void unload();
		void destroy() override;
		~AssetPack();
	private:
		bool m_loaded;
		std::unordered_map<std::string, Asset*> m_assets;
	};

	/**
	 * @brief A collection of a few defualt assets included in `base.nfpack`
	 * 
	 * All models have a default, grey material.
	*/
	struct BaseAssets {
		/**
		 * @brief A cube
		*/
		static AModel* cube;
		/**
		 * @brief A flattened cube
		*/
		static AModel* plane;
		/**
		 * @brief A sphere
		*/
		static AModel* sphere;
		/**
		 * @brief A cone
		*/
		static AModel* cone;
		/**
		 * @brief A cylinder
		*/
		static AModel* cylinder;
		/**
		 * @brief A torus
		*/
		static AModel* torus;
		/**
		 * @brief NF logo
		*/
		static ATexture* logo;
		/**
		 * @brief Default cubemap
		*/
		static ACubemap* cubemap;
		/**
		 * @brief Default font
		 * 
		 * The default font is Segoe UI Light.
		*/
		static AFont* font;
		/**
		 * @brief Default button textures
		 * 
		 * @sa @ref customButtons
		*/
		static AButton* button;
	};
}
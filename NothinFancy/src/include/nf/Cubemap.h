#pragma once
#include "nf/NFObject.h"
#include "nf/Drawable.h"

namespace nf {
	struct Asset;
	class Shader;

	/**
	 * @brief A skybox in the background of a gamestate's world
	 * 
	 * A cubemap is a cube with a texture on each one of its 6 sides.
	 * 
	 * @sa @ref createCubemapTut
	 * @ref customCubemaps
	*/
	class Cubemap : public Drawable, public NFObject {
	public:
		/**
		 * @brief Constructor
		 * 
		 * See @ref obLifetime
		*/
		Cubemap();
		/**
		 * @brief Creates a cubemap
		 * @param cubemapAsset A cubemap asset retrieved from an AssetPack
		 * 
		 * This function will prepare the cubemap to be rendered.
		 * 
		 * @warning Trying to render a cubemap before creating it will result in an error.
		*/
		void create(Asset* cubemapAsset);
		/**
		 * @brief Queries whether or not the cubemap has been created
		 * @return If the cubemap has been created
		*/
		bool isConstructed();
#ifndef NFIMPL
		void render(Shader* shader);
#endif
		void destroy() override;
		~Cubemap();
	private:
		bool m_constructed;
		unsigned int m_id;
	};
}
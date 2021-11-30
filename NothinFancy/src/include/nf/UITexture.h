#pragma once
#include "nf/UIElement.h"
#include "nf/NFObject.h"
#include "nf/Utility.h"

namespace nf {
	class Texture;
	struct Asset;

	/**
	 * @brief A UI image
	 * 
	 * This class represents a texture that can be added to the UI.
	*/
	class UITexture : public UIElement, public NFObject {
	public:
		/**
		 * @brief Constructor
		 * 
		 * See @ref obLifetime
		*/
		UITexture();
		/**
		 * @brief Creates a UI texture
		 * @param textureAsset Texture asset retrieved from an AssetPack
		 * @param position Position vector in screen space
		 * @param scale Scale of the texture
		 * @param opacity Opacity of the texture
		*/
		void create(Asset* textureAsset, const Vec2& position, float scale = 1.0f, float opacity = 1.0f);
		/**
		 * @brief Sets the texture's scale
		 * @param scale The new scale
		*/
		void setScale(float scale);
		/**
		 * @brief Sets the texture's opacity
		 * @param opacity The new opacity
		*/
		void setOpacity(float opacity);
#ifndef NFIMPL
		const char* identity() override;
		void render(Shader* shader, unsigned int windowWidth, unsigned int windowHeight) override;
#endif
		void destroy() override;
		~UITexture();
	private:
		Texture* m_texture;
		float m_scale;
		float m_opacity;
	};
}
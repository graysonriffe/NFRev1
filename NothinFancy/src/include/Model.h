#pragma once
#include "Drawable.h"
#include "Assets.h"

namespace nf {
	class Drawable;
	class Texture;

	class Model : public Drawable {
	public:
		Model();

		void create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount, const void* textureCoordinatesBufferData, size_t textureCoordinatesBufferSize, const void* normalsBufferData, size_t normalsBufferSize, ATexture* texture);
		void bind() override;

		void setBaseAsset(bool isBase);
		bool isBaseAsset();

		~Model();
	private:
		bool m_base;
		Texture* m_texture;
	};
}
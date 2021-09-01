#pragma once
#include "Drawable.h"

namespace nf {
	class Drawable;
	class Texture;
	class Model : public Drawable {
	public:
		Model();

		void create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount, const void* textureCoordinates, size_t textureCoordinatesBufferSize, const unsigned char* textureData, size_t textureSize);
		void bind() override;

		~Model();
	private:
		Texture* m_texture;
	};
}
#pragma once
#ifdef NFENGINE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#endif

#include "Drawable.h"
#include "Texture.h"

namespace nf {
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
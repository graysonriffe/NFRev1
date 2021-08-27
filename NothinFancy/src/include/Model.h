#pragma once
#ifdef NFENGINE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#endif

#include "Drawable.h"

namespace nf {
	class Model : public Drawable {
	public:
		Model();

		void create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount);
		void bind() override;

		~Model();
	private:

	};
}
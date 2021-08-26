#pragma once
#include "Drawable.h"

namespace nf {
	class Model : public Drawable {
	public:
		Model();

		void create(const void* vertexBufferData, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount, const char* vertexShader = nullptr, const char* fragmentShader = nullptr);
		DrawableType identity() override;
		void bind() override;
		bool hasCustomShader();

		~Model();
	private:

	};
}
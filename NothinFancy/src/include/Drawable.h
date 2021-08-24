#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"

namespace nf {
	class Drawable {
		enum class DrawableType {
			NF_GAME, NF_UI
		};
	public:
		Drawable();

		virtual DrawableType identity();

		~Drawable();
	protected:
		//TODO: Add VAO, Shader, index buffer, etc.
	};
}
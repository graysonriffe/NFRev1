#pragma once
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

namespace nf {
	class Drawable {
	public:
		enum class DrawableType {
			NF_NONE, NF_GAME, NF_UI
		};
		//TODO: Construct using Shader code and data from obj
		Drawable();

		virtual DrawableType identity();
		virtual void bind();
		unsigned int getIndexCount();

		~Drawable();
	protected:
		VertexArray* m_vao;
		IndexBuffer* m_ib;
		Shader* m_shader;
	};
}
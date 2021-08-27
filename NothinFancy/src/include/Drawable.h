#pragma once
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

namespace nf {
	class Drawable {
	public:
		Drawable();

		virtual void bind();
		unsigned int getIndexCount();

		~Drawable();
	protected:
		VertexArray* m_vao;
		IndexBuffer* m_ib;
	};
}
#pragma once
#include "nf/VertexArray.h"
#include "nf/IndexBuffer.h"

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
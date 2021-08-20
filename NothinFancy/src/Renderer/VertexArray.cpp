#include "VertexArray.h"

namespace nf{
	VertexArray::VertexArray(VertexBuffer& buffer) {
		glGenVertexArrays(1, &m_id);
		//TODO: Bind buffer and set the layout
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &m_id);
	}
}
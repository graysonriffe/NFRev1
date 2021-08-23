#include "VertexArray.h"

namespace nf {
	unsigned int VertexBufferElement::getSizeOfType(unsigned int type) {
		switch (type)
		{
		case GL_FLOAT:
			return sizeof(type);
		default:
			return 0;
		}
	}

	VertexArray::VertexArray(const void* bufferData, size_t bufferSize) :
		m_id(0),
		m_vb(bufferData, bufferSize),
		m_hasLayout(false),
		m_vertexStride(0)
	{
		glGenVertexArrays(1, &m_id);
	}

	void VertexArray::bind() {
		glBindVertexArray(m_id);
		m_vb.bind();
		if (!m_hasLayout && m_vertexStride > 0) {
			unsigned int offset = 0;
			for (unsigned int i = 0; i < m_layoutElements.size(); i++) {
				const VertexBufferElement& element = m_layoutElements[i];
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, element.count, element.type, element.normalized, m_vertexStride, (const void*)offset);
				offset += element.count * VertexBufferElement::getSizeOfType(element.type);
			}
			m_hasLayout = true;
		}
		else if(!m_hasLayout) {
			Error("No layout specified for vertex buffer!");
		}
	}

	template<>
	void VertexArray::push<float>(unsigned int count) {
		m_layoutElements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_vertexStride += VertexBufferElement::getSizeOfType(GL_FLOAT) * count;
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &m_id);
	}
}
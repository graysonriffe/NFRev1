#pragma once
#ifdef NFENGINE
#include "GL/glew.h"
#endif
#include <vector>

#include "VertexBuffer.h"
#include "Utility.h"

namespace nf {
	struct VertexBufferElement {
		unsigned int type;
		unsigned int count;
		unsigned char normalized;

		static unsigned int getSizeOfType(unsigned int type);
	};

	class VertexArray {
	public:
		VertexArray(const void* bufferData, size_t bufferSize);

		void bind();
		template<typename T>
		void push(unsigned int count);

		~VertexArray();
	private:
		unsigned int m_id;
		VertexBuffer m_vb;
		bool m_hasLayout;
		std::vector<VertexBufferElement> m_layoutElements;
		unsigned int m_vertexStride;
	};
}
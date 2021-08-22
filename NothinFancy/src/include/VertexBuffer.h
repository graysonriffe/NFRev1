#pragma once
#ifdef NFENGINE
#include "GL/glew.h"
#endif

namespace nf {
	class VertexBuffer {
	public:
		VertexBuffer(const void* data, size_t size);

		void bind() const;

		~VertexBuffer();
	private:
		unsigned int m_id;
	};
}
#pragma once
#include "VertexBuffer.h"

namespace nf {
	class VertexArray {
	public:
		VertexArray(VertexBuffer& buffer);

		~VertexArray();
	private:
		unsigned int m_id;
	};
}
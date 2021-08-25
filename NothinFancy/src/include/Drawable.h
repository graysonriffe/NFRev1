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
		Drawable(const char* vertexShader, const char* fragmentShader, const void* vertexBuffer, const size_t vertexBufferSize, const void* indexBufferData, size_t indexBufferCount);

		unsigned int getIndexCount();
		void bind();
		virtual DrawableType identity();

		~Drawable();
	protected:
		//TODO: Add VAO, Shader, index buffer, etc.
		Shader m_shader;
		VertexArray m_vao;
		IndexBuffer m_ib;
	};
}
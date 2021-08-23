#pragma once
#ifdef NFENGINE
#include "GL/glew.h"
#endif

#include "Utility.h"

namespace nf {
	class Shader {
	public:
		Shader(const char* vertexSource, const char* fragmentSource);

		void bind();

		~Shader();
	private:
		unsigned int m_id;
		//Associated resource?
	};
}
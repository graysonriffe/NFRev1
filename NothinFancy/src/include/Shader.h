#pragma once
#include <unordered_map>
#ifdef NFENGINE
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#endif

namespace nf {
	class Shader {
	public:
		Shader(const char* vertexSource, const char* fragmentSource);

		void bind();
		void setUniform(const char* name, glm::mat4& data);
		static unsigned int current;

		~Shader();
	private:
		void getUniformLocation(const char* uniformName);

		unsigned int m_id;
		std::unordered_map<const char*, unsigned int> m_uniformLocations;
	};
}
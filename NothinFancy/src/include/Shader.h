#pragma once
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace nf {
	class Shader {
	public:
		Shader(const char* vertexSource, const char* fragmentSource);

		void bind();
		void setUniform(const char* name, glm::mat4& data);
		void setUniform(const char* name, glm::vec3& data);

		~Shader();
	private:
		void getUniformLocation(const char* uniformName);

		unsigned int m_id;
		std::unordered_map<const char*, unsigned int> m_uniformLocations;
	};
}
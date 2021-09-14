#pragma once
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace nf {
	class Shader {
	public:
		Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);

		void bind();
		void setUniform(const std::string& name, glm::mat4& data);
		void setUniform(const std::string& name, glm::vec3& data);
		void setUniform(const std::string& name, int data);
		void setUniform(const std::string& name, float data);

		~Shader();
	private:
		void getUniformLocation(const std::string& uniformName);

		unsigned int m_id;
		std::unordered_map<std::string, unsigned int> m_uniformLocations;
	};
}
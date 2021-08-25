#pragma once
#include <unordered_map>

namespace nf {
	class Shader {
	public:
		Shader(const char* vertexSource, const char* fragmentSource);

		void bind();
		void getUniformLocation(const char* uniformName);

		static unsigned int current;

		~Shader();
	private:
		unsigned int m_id;
		std::unordered_map<const char*, unsigned int> m_uniformLocations;
		//TODO: Load from resource
	};
}
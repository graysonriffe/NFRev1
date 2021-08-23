#include "Shader.h"

namespace nf {
	Shader::Shader(const char* vertexSource, const char* fragmentSource) {
		m_id = glCreateProgram();
		unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vs, 1, &vertexSource, nullptr);
		glShaderSource(fs, 1, &fragmentSource, nullptr);
		glCompileShader(vs);
		glCompileShader(fs);
		for (int i = 0; i < 2; i++) {
			unsigned int curr = (i == 0 ? vs : fs);
			int result;
			glGetShaderiv(curr, GL_COMPILE_STATUS, &result);
			if (result != GL_TRUE) {
				int length;
				glGetShaderiv(curr, GL_INFO_LOG_LENGTH, &length);
				char* message = new char[length];
				glGetShaderInfoLog(curr, length, &length, message);
				message[length - 2] = 0;
				Error(("OpenGL Error: " + (std::string)message).c_str());
			}
		}
		glAttachShader(m_id, vs);
		glAttachShader(m_id, fs);
		glLinkProgram(m_id);
		glValidateProgram(m_id);
		int result;
		glGetProgramiv(m_id, GL_VALIDATE_STATUS, &result);
		if (result != GL_TRUE) {
			int length;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
			char* message = new char[length];
			glGetProgramInfoLog(m_id, length, &length, message);
			Error(("OpenGL Error: " + (std::string)message).c_str());
		}
		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	void Shader::bind() {
		glUseProgram(m_id);
	}

	Shader::~Shader() {
		glDeleteProgram(m_id);
	}
}
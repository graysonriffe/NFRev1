#include "nf/Shader.h"

#include "GL/glew.h"

#include "nf/Utility.h"

namespace nf {
	Shader::Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource) {
		m_id = glCreateProgram();
		unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
		unsigned int gs = 0;
		glShaderSource(vs, 1, &vertexSource, nullptr);
		glShaderSource(fs, 1, &fragmentSource, nullptr);
		glCompileShader(vs);
		glCompileShader(fs);
		if (geometrySource) {
			gs = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(gs, 1, &geometrySource, nullptr);
			glCompileShader(gs);
		}
		for (int i = 0; i < 3; i++) {
			unsigned int curr;
			if (i == 0)
				curr = vs;
			else if (i == 1)
				curr = fs;
			else if (i == 2)
				curr = gs;
			if (curr == 0)
				break;
			int result;
			glGetShaderiv(curr, GL_COMPILE_STATUS, &result);
			if (result != GL_TRUE) {
				int length;
				glGetShaderiv(curr, GL_INFO_LOG_LENGTH, &length);
				char* message = new char[length];
				glGetShaderInfoLog(curr, length, &length, message);
				message[length - 2] = 0;
				NFError("OpenGL Error: " + (std::string)message);
			}
		}
		glAttachShader(m_id, vs);
		glAttachShader(m_id, fs);
		if (gs) glAttachShader(m_id, gs);
		glLinkProgram(m_id);
		int result;
		glGetProgramiv(m_id, GL_LINK_STATUS, &result);
		if (result != GL_TRUE) {
			int length;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
			char* message = new char[length];
			glGetProgramInfoLog(m_id, length, &length, message);
			NFError("OpenGL Error: " + (std::string)message);
		}
		glDetachShader(m_id, vs);
		glDetachShader(m_id, fs);
		glDeleteShader(vs);
		glDeleteShader(fs);
		if (gs) {
			glDetachShader(m_id, gs);
			glDeleteShader(gs);
		}
	}

	void Shader::validate() {
		glValidateProgram(m_id);
		int result;
		glGetProgramiv(m_id, GL_VALIDATE_STATUS, &result);
		if (result != GL_TRUE) {
			int length;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
			char* message = new char[length];
			glGetProgramInfoLog(m_id, length, &length, message);
			NFError("OpenGL Error: " + (std::string)message);
		}
	}

	void Shader::bind() {
		glUseProgram(m_id);
	}

	void Shader::setUniform(const std::string& name, glm::mat4& data) {
		bind();
		if (m_uniformLocations.find(name) == m_uniformLocations.end())
			getUniformLocation(name);
		glUniformMatrix4fv(m_uniformLocations[name], 1, GL_FALSE, glm::value_ptr(data));
	}
	void Shader::setUniform(const std::string& name, glm::vec3& data) {
		bind();
		if (m_uniformLocations.find(name) == m_uniformLocations.end())
			getUniformLocation(name);
		glUniform3fv(m_uniformLocations[name], 1, glm::value_ptr(data));
	}
	void Shader::setUniform(const std::string& name, int data) {
		bind();
		if (m_uniformLocations.find(name) == m_uniformLocations.end())
			getUniformLocation(name);
		glUniform1i(m_uniformLocations[name], data);
	}
	void Shader::setUniform(const std::string& name, float data) {
		bind();
		if (m_uniformLocations.find(name) == m_uniformLocations.end())
			getUniformLocation(name);
		glUniform1f(m_uniformLocations[name], data);
	}

	void Shader::getUniformLocation(const std::string& uniformName) {
		unsigned int loc = glGetUniformLocation(m_id, uniformName.c_str());
		if (loc == -1)
			NFError("Uniform \"" + (std::string)uniformName + "\" does not exist!");
		m_uniformLocations[uniformName] = loc;
	}

	Shader::~Shader() {
		glDeleteProgram(m_id);
		m_uniformLocations.clear();
	}
}
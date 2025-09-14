#include "ShaderProgram.h"
#include "Renderer.h"
#include "../LogSystem.h"

namespace IcePick {

	ShaderProgram::ShaderProgram() {
		m_ShaderProgramID = 0;
	}

	ShaderProgram::ShaderProgram(const ShaderProgram& other) {
		m_ShaderProgramID = other.m_ShaderProgramID;
	}

	void ShaderProgram::CompileShaderProgram(const ShaderSource& programSource) {
		m_ShaderProgramID = glCreateProgram();
		IP_LOG("Created shader program ID: " + std::to_string(m_ShaderProgramID));
		unsigned int vertexShaderId = CompilerShader(GL_VERTEX_SHADER, programSource.VertexShaderSource);
		unsigned int fragmentShaderId = CompilerShader(GL_FRAGMENT_SHADER, programSource.FragmentShaderSource);
		
		glAttachShader(m_ShaderProgramID, vertexShaderId);
		glAttachShader(m_ShaderProgramID, fragmentShaderId);
		glLinkProgram(m_ShaderProgramID);
		glValidateProgram(m_ShaderProgramID);

		// TODO: Error handling

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
	}

	void ShaderProgram::Bind() {
		glUseProgram(m_ShaderProgramID);
	}

	void ShaderProgram::UnBind() {
		glUseProgram(0);
	}

	void ShaderProgram::Destroy() {
		glDeleteProgram(m_ShaderProgramID);
	}

	unsigned int ShaderProgram::CompilerShader(unsigned int shaderType, const std::string& shaderSource) {
		unsigned int shaderId = glCreateShader(shaderType);
		const char* src = shaderSource.c_str();
		glShaderSource(shaderId, 1, &src, nullptr);
		glCompileShader(shaderId);

		int result;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
			char* err = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(shaderId, length, &length, err);
			IP_LOG("Failed to compile shader.", IP_ERROR_LOG);
			IP_LOG(err, IP_ERROR_LOG);
			glDeleteShader(shaderId);
			return 0;
		}

		return shaderId;
	}

	ShaderProgram::~ShaderProgram() {

	}
}
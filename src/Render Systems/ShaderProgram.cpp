#include "ShaderProgram.h"
#include "Renderer.h"
#include "../LogSystem.h"

namespace IcePick {

	ShaderProgram::ShaderProgram() {
		m_ShaderProgramID = 0;
	}

	ShaderProgram::ShaderProgram(const ShaderProgram& other) {
		m_ShaderProgramID = other.m_ShaderProgramID;
		m_ShaderProgramValid = other.m_ShaderProgramValid;
		m_CachedUniformLocations.clear();
		m_CachedUniformLocations = other.m_CachedUniformLocations;
	}

	unsigned int ShaderProgram::GetID() {
		return m_ShaderProgramID;
	}

	void ShaderProgram::CompileShaderProgram(const ShaderSource& programSource) {
		m_ShaderProgramID = glCreateProgram();
		unsigned int vertexShaderId = CompilerShader(GL_VERTEX_SHADER, programSource.VertexShaderSource);
		unsigned int fragmentShaderId = CompilerShader(GL_FRAGMENT_SHADER, programSource.FragmentShaderSource);
		
		glAttachShader(m_ShaderProgramID, vertexShaderId);
		glAttachShader(m_ShaderProgramID, fragmentShaderId);
		glLinkProgram(m_ShaderProgramID);
		glValidateProgram(m_ShaderProgramID);

		// TODO: Error handling
		if (vertexShaderId && fragmentShaderId)
			m_ShaderProgramValid = true;

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
	}

	bool ShaderProgram::IsValid() {
		return m_ShaderProgramValid;
	}

	void ShaderProgram::Use() {
		glUseProgram(m_ShaderProgramID);
	}

	void ShaderProgram::UnBind() {
		glUseProgram(0);
	}

	void ShaderProgram::Destroy() {
		glDeleteProgram(m_ShaderProgramID);
		m_ShaderProgramValid = false;
		m_ShaderProgramID = 0;
		m_CachedUniformLocations.clear();
	}

	inline void ShaderProgram::SetShaderUniform(int location, uint32_t value) {
		glUniform1ui(location, value);
	}

	void ShaderProgram::RegisterSetUniformUint32(const char* uniform, uint32_t value) {
		int location = glGetUniformLocation(m_ShaderProgramID, uniform);
		if (location < 0) {
			IP_LOG("Failed to get location for uniform: " + std::string(uniform), IP_ERROR_LOG);
			return;
		}

		m_CachedUniformLocations.insert({ uniform, location });
		SetShaderUniform(location, value);
	}

	void ShaderProgram::SetUniformUint32(const char* uniform, uint32_t value) {
		Use();
		auto iterator = m_CachedUniformLocations.find(uniform);

		if (iterator != m_CachedUniformLocations.end()) {
			int location = iterator->second;			
			SetShaderUniform(location, value);
			return;
		}

		RegisterSetUniformUint32(uniform, value);
		UnBind();
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
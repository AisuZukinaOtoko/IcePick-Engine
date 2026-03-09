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

	int ShaderProgram::GetUniformLocation(const char* uniform) {
		int location = glGetUniformLocation(m_ShaderProgramID, uniform);
		if (location < 0) {
			IP_LOG("Failed to get location for uniform: " + std::string(uniform), IP_ERROR_LOG);
		}
		return location;
	}

	inline void ShaderProgram::UploadShaderUniformUint32(int location, uint32_t value) {
		glUniform1ui(location, value);
	}

	inline void ShaderProgram::UploadShaderUniformInt32(int location, int32_t value) {
		glUniform1i(location, value);
	}

	inline void ShaderProgram::UploadShaderUniformFloat(int location, float value) {
		glUniform1f(location, value);
	}

	inline void ShaderProgram::UploadShaderUniformMat4(int location, glm::mat4& value) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}

	inline void ShaderProgram::UploadShaderUniformMat3(int location, glm::mat3& value) {
		glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
	}

	inline void ShaderProgram::UploadShaderUniformVec3(int location, glm::vec3& value) {
		glUniform3fv(location, 1, &value[0]);
	}

	void ShaderProgram::RegisterSetUniformUint32(const char* uniform, uint32_t value) {
		int location = GetUniformLocation(uniform);
		if (location < 0) {
			return;
		}

		m_CachedUniformLocations.insert({ uniform, location });
		UploadShaderUniformUint32(location, value);
	}

	void ShaderProgram::RegisterSetUniformInt32(const char* uniform, int32_t value) {
		int location = GetUniformLocation(uniform);
		if (location < 0) {
			return;
		}

		m_CachedUniformLocations.insert({ uniform, location });
		UploadShaderUniformInt32(location, value);
	}

	void ShaderProgram::RegisterSetUniformFloat(const char* uniform, float value) {
		int location = GetUniformLocation(uniform);
		if (location < 0) {
			return;
		}

		m_CachedUniformLocations.insert({ uniform, location });
		UploadShaderUniformFloat(location, value);
	}

	void ShaderProgram::RegisterSetUniformMat4(const char* uniform, glm::mat4& value) {
		int location = GetUniformLocation(uniform);
		if (location < 0) {
			return;
		}

		m_CachedUniformLocations.insert({ uniform, location });
		UploadShaderUniformMat4(location, value);
	}

	void ShaderProgram::RegisterSetUniformMat3(const char* uniform, glm::mat3& value) {
		int location = GetUniformLocation(uniform);
		if (location < 0) {
			return;
		}

		m_CachedUniformLocations.insert({ uniform, location });
		UploadShaderUniformMat3(location, value);
	}

	void ShaderProgram::RegisterSetUniformVec3(const char* uniform, glm::vec3& value) {
		int location = GetUniformLocation(uniform);
		if (location < 0) {
			return;
		}

		m_CachedUniformLocations.insert({ uniform, location });
		UploadShaderUniformVec3(location, value);
	}

	void ShaderProgram::SetUniformUint32(const char* uniform, uint32_t value) {
		Use();
		auto iterator = m_CachedUniformLocations.find(uniform);

		if (iterator != m_CachedUniformLocations.end()) {
			int location = iterator->second;			
			UploadShaderUniformUint32(location, value);
			return;
		}

		RegisterSetUniformUint32(uniform, value);
		UnBind();
	}

	void ShaderProgram::SetUniformInt32(const char* uniform, int32_t value) {
		Use();
		auto iterator = m_CachedUniformLocations.find(uniform);

		if (iterator != m_CachedUniformLocations.end()) {
			int location = iterator->second;
			UploadShaderUniformInt32(location, value);
			return;
		}

		RegisterSetUniformInt32(uniform, value);
		UnBind();
	}

	void ShaderProgram::SetUniformFloat(const char* uniform, float value) {
		Use();
		auto iterator = m_CachedUniformLocations.find(uniform);

		if (iterator != m_CachedUniformLocations.end()) {
			int location = iterator->second;
			UploadShaderUniformFloat(location, value);
			return;
		}

		RegisterSetUniformFloat(uniform, value);
		UnBind();
	}

	void ShaderProgram::SetUniformMat4(const char* uniform, glm::mat4& value) {
		Use();
		auto iterator = m_CachedUniformLocations.find(uniform);

		if (iterator != m_CachedUniformLocations.end()) {
			int location = iterator->second;
			UploadShaderUniformMat4(location, value);
			return;
		}

		RegisterSetUniformMat4(uniform, value);
		UnBind();
	}

	void ShaderProgram::SetUniformMat3(const char* uniform, glm::mat3& value) {
		Use();
		auto iterator = m_CachedUniformLocations.find(uniform);

		if (iterator != m_CachedUniformLocations.end()) {
			int location = iterator->second;
			UploadShaderUniformMat3(location, value);
			return;
		}

		RegisterSetUniformMat3(uniform, value);
		UnBind();
	}

	void ShaderProgram::SetUniformVec3(const char* uniform, glm::vec3& value) {
		Use();
		auto iterator = m_CachedUniformLocations.find(uniform);

		if (iterator != m_CachedUniformLocations.end()) {
			int location = iterator->second;
			UploadShaderUniformVec3(location, value);
			return;
		}

		RegisterSetUniformVec3(uniform, value);
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
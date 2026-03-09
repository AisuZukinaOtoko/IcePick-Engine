#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace IcePick {
	struct ShaderSource {
		std::string VertexShaderSource;
		std::string FragmentShaderSource;
	};

	class ShaderProgram {
	public:
		ShaderProgram();
		ShaderProgram(const ShaderProgram& other);
		~ShaderProgram();
		unsigned int GetID();
		void CompileShaderProgram(const ShaderSource& programSource);
		bool IsValid();
		void Use();
		void UnBind();
		void Destroy();

		// Uniforms
		void SetUniformUint32(const char* uniform, uint32_t value);
		void SetUniformInt32(const char* uniform, int32_t value);
		void SetUniformFloat(const char* uniform, float value);
		void SetUniformMat4(const char* uniform, glm::mat4& value);
		void SetUniformMat3(const char* uniform, glm::mat3& value);
		void SetUniformVec3(const char* uniform, glm::vec3& value);
	private:
		unsigned int CompilerShader(unsigned int shaderType, const std::string& shaderSource);
		unsigned int m_ShaderProgramID = 0;
		bool m_ShaderProgramValid = false;

		int GetUniformLocation(const char* uniform);

		std::unordered_map<std::string, int> m_CachedUniformLocations;
		inline void UploadShaderUniformUint32(int location, uint32_t value);
		inline void UploadShaderUniformInt32(int location, int32_t value);
		inline void UploadShaderUniformFloat(int location, float value);
		inline void UploadShaderUniformMat4(int location, glm::mat4& value);
		inline void UploadShaderUniformMat3(int location, glm::mat3& value);
		inline void UploadShaderUniformVec3(int location, glm::vec3& value);

		void RegisterSetUniformUint32(const char* uniform, uint32_t value);
		void RegisterSetUniformInt32(const char* uniform, int32_t value);
		void RegisterSetUniformFloat(const char* uniform, float value);
		void RegisterSetUniformMat4(const char* uniform, glm::mat4& value);
		void RegisterSetUniformMat3(const char* uniform, glm::mat3& value);
		void RegisterSetUniformVec3(const char* uniform, glm::vec3& value);
	};
}
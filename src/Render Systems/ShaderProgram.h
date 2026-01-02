#pragma once
#include <string>
#include <unordered_map>

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
	private:
		unsigned int CompilerShader(unsigned int shaderType, const std::string& shaderSource);
		unsigned int m_ShaderProgramID = 0;
		bool m_ShaderProgramValid = false;

		std::unordered_map<std::string, int> m_CachedUniformLocations;
		inline void SetShaderUniformUint32(int location, uint32_t value);
		inline void SetShaderUniformInt32(int location, int32_t value);
		inline void SetShaderUniformFloat(int location, float value);
		void RegisterSetUniformUint32(const char* uniform, uint32_t value);
		void RegisterSetUniformInt32(const char* uniform, int32_t value);
		void RegisterSetUniformFloat(const char* uniform, float value);
	};
}
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
	private:
		unsigned int CompilerShader(unsigned int shaderType, const std::string& shaderSource);
		unsigned int m_ShaderProgramID = 0;
		bool m_ShaderProgramValid = false;

		std::unordered_map<std::string, int> m_CachedUniformLocations;
		inline void SetShaderUniform(int location, uint32_t value);
		void RegisterSetUniformUint32(const char* uniform, uint32_t value);
	};
}
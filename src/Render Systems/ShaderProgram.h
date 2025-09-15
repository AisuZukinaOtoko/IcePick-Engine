#pragma once
#include <string>

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
	private:
		unsigned int CompilerShader(unsigned int shaderType, const std::string& shaderSource);
		unsigned int m_ShaderProgramID = 0;
		bool m_ShaderProgramValid = false;
	};
}
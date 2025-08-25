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
		void CompileShaderProgram(const ShaderSource& programSource);
		void Bind();
		void UnBind();
		void Destroy();
	private:
		unsigned int CompilerShader(unsigned int shaderType, const std::string& shaderSource);
		unsigned int m_ShaderProgramID = 0;
	};
}
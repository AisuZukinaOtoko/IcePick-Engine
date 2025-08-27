#include "ShaderLoader.h"
#include "../LogSystem.h"
#include <fstream>
#include <sstream>

namespace IcePick {

	ShaderLoader::ShaderLoader() {
		ShaderSource shaderSource;
		shaderSource.VertexShaderSource = LoadFile("res/shaders/pbr.vert.shader", 0);
		shaderSource.FragmentShaderSource = LoadFile("res/shaders/pbr.frag.shader", 0);

		ShaderProgram pbrShader;
		//pbrShader
		IP_LOG(shaderSource.FragmentShaderSource);
		m_DefaultShaderPrograms[PBR_SHADER_PROGRAM].CompileShaderProgram(shaderSource);
	}

	void ShaderLoader::RegisterShaderProgram(ShaderProgram shaderProgram) {

	}

	std::string ShaderLoader::LoadFile(std::filesystem::path filePath, unsigned int includeDepth) {
		IP_LOG(filePath.string(), IP_STANDARD_LOG);
		if (includeDepth >= 10) {
			IP_LOG("Maximum include depth for shader includes.", IP_ERROR_LOG);
			return "";
		}

		std::filesystem::path parentDirectory = filePath.parent_path();
		std::ifstream inFile(filePath);

		if (!inFile.is_open()) {
			IP_LOG("Could not open shader file: " + filePath.string(), IP_ERROR_LOG);
			return "";
		}

		std::stringstream shaderStream;
		std::string line;

		while (std::getline(inFile, line)) {
			size_t includeIndex = line.find("#include");

			if (includeIndex != std::string::npos) {
				size_t startQuote = line.find('"');
				size_t endQuote = line.find('"', startQuote + 1);

				// Nothing to include, empties line to avoid shader compile error
				if (startQuote == std::string::npos || endQuote == std::string::npos) {
					IP_LOG("Invalid include in shader: " + filePath.string(), IP_ERROR_LOG);
					line = "";
				}
				else {
					std::string includeFile = line.substr(startQuote + 1, endQuote - startQuote - 1);
					std::filesystem::path includePath = parentDirectory / includeFile;
					line = LoadFile(std::filesystem::canonical(includePath), includeDepth + 1);
				}

			}

			shaderStream << line << '\n';
		}

		return shaderStream.str();
	}

	UUID ShaderLoader::LoadShaderProgram(std::filesystem::path shaderPath) {
		return UUID();
	}

	void ShaderLoader::ReloadShaderProgram(UUID shaderId) {

	}

	ShaderProgram ShaderLoader::GetShaderProgram(UUID shaderId) {
		return ShaderProgram();
	}

	ShaderProgram ShaderLoader::GetDefaultShaderProgram(DefaultShaderTypes shaderType) {
		return m_DefaultShaderPrograms[shaderType];
	}

	void ShaderLoader::ShutDown() {

	}

	ShaderLoader::~ShaderLoader() {

	}
	
}
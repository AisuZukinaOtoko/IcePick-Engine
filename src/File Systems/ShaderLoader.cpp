#include "ShaderLoader.h"
#include "../LogSystem.h"
#include <fstream>
#include <sstream>

namespace IcePick {

	ShaderLoader::ShaderLoader() {
		ShaderSource shaderSource;
		shaderSource.VertexShaderSource = LoadFile("res/shaders/pbr.vert.shader", 0);
		shaderSource.FragmentShaderSource = LoadFile("res/shaders/pbr.frag.shader", 0);
		IP_LOG(shaderSource.VertexShaderSource);
		m_DefaultShaderPrograms[PBR_SHADER_PROGRAM].CompileShaderProgram(shaderSource);
	}

	void ShaderLoader::RegisterShaderProgram(ShaderProgram shaderProgram) {

	}

	std::string ShaderLoader::LoadFile(std::filesystem::path filePath, unsigned int includeDepth) {
		std::filesystem::path parentDirectory = filePath.parent_path();
		std::ifstream inFile(filePath);

		if (!inFile.is_open()) {
			IP_LOG("Could not open shader file: " + filePath.string(), IP_ERROR_LOG);
			return "";
		}

		std::stringstream shaderStream;
		std::string line;

		while (std::getline(inFile, line)) {
			if (line.include)
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
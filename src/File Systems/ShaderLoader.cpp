#include "ShaderLoader.h"
#include "../LogSystem.h"
#include <fstream>
#include <sstream>

namespace IcePick {

	ShaderLoader::ShaderLoader() {
	
	}

	void ShaderLoader::SetDefaultShaderProgram(ShaderProgram shaderProgram)	{
		m_DefaultShaderProgram = shaderProgram;
		m_CachedShaderProgram = m_DefaultShaderProgram;
	}

	UUID ShaderLoader::RegisterShaderProgram(ShaderProgram shaderProgram) {
		UUID shaderId;
		m_LoadedShaders.insert({ shaderId, shaderProgram });
		return shaderId;
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

	UUID ShaderLoader::CreateShaderProgram(ShaderSource& shaderSource) {
		ShaderProgram shader;
		shader.CompileShaderProgram(shaderSource);

		if (!shader.IsValid()) {
			shader.Destroy();
			return UUID::Unitialised();
		}

		return RegisterShaderProgram(shader);
	}

	void ShaderLoader::ReloadShaderProgram(UUID shaderId, ShaderSource& shaderSource) {
		auto iterator = m_LoadedShaders.find(shaderId);

		// Shader program not found, so do not reload shader and do not create a new shader
		if (iterator == m_LoadedShaders.end())
			return;

		ShaderProgram newShader;
		newShader.CompileShaderProgram(shaderSource);

		// Keep old shader if new shader is invalid
		if (!newShader.IsValid()) {
			newShader.Destroy();
			return;
		}
		
		iterator->second.Destroy();
		iterator->second = newShader;
	}

	ShaderProgram ShaderLoader::GetShaderProgram(UUID shaderId) {
		if (shaderId == UUID::Unitialised())
			return m_DefaultShaderProgram;

		if (shaderId == m_CachedShaderProgramId)
			return m_CachedShaderProgram;

		auto iterator = m_LoadedShaders.find(shaderId);

		if (iterator == m_LoadedShaders.end())
			return m_DefaultShaderProgram;

		m_CachedShaderProgramId = shaderId;
		m_CachedShaderProgram = iterator->second;
		return iterator->second;
	}

	void ShaderLoader::ShutDown() {
		for (auto iterator = m_LoadedShaders.begin(); iterator != m_LoadedShaders.end(); ++iterator) {
			iterator->second.Destroy();
		}
		m_LoadedShaders.clear();
		m_LoadedShaderPaths.clear();
	}

	ShaderLoader::~ShaderLoader() {

	}
	
}
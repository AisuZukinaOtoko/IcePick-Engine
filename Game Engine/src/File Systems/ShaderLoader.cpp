#include "ShaderLoader.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"
#include <fstream>
#include <sstream>

namespace IcePick {

	ShaderLoader::ShaderLoader() {
	
	}

	void ShaderLoader::Init() {
		ShaderSource lineShaderSource;
		lineShaderSource.VertexShaderSource = LoadFile("Game Engine/res/Shaders/line.vert.shader", 0);
		lineShaderSource.FragmentShaderSource = LoadFile("Game Engine/res/Shaders/line.frag.shader", 0);
		UUID lineShaderId = CreateShaderProgram(lineShaderSource);
		DefaultSahderIds[LINE_SHADER] = lineShaderId;
	}

	void ShaderLoader::SetDefaultShaderProgram(ShaderProgram shaderProgram)	{
		m_DefaultShaderProgram = shaderProgram;
		m_CachedShaderProgram = &m_DefaultShaderProgram;
	}

	UUID ShaderLoader::RegisterShaderProgram(ShaderProgram shaderProgram) {
		UUID shaderId;
		m_LoadedShaders.insert({ shaderId, shaderProgram });
		return shaderId;
	}

	std::string ShaderLoader::LoadFile(std::filesystem::path filePath, unsigned int includeDepth) {
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

	void ShaderLoader::CreateShaderProgramWithId(ShaderSource& shaderSource, UUID Id) {
		ShaderProgram shader;
		shader.CompileShaderProgram(shaderSource);

		if (!shader.IsValid()) {
			shader.Destroy();
			return;
		}

		m_LoadedShaders.insert({ Id, shader });
	}

	void ShaderLoader::ReloadShaderProgram(UUID shaderId, ShaderSource& shaderSource) {
		auto iterator = m_LoadedShaders.find(shaderId);

		// Shader program not found, so do not reload shader and do not create a new shader
		if (iterator == m_LoadedShaders.end()) {
			IP_LOG("Cannot reload shader. Shader not found.", IP_WARN_LOG);
			return;
		}

		ShaderProgram newShader;
		newShader.CompileShaderProgram(shaderSource);

		// Keep old shader if new shader is invalid
		if (!newShader.IsValid()) {
			newShader.Destroy();
			return;
		}
		
		iterator->second.Destroy();
		iterator->second = newShader;

		if (m_CachedShaderProgramId == shaderId)
			m_CachedShaderProgram = &newShader;
	}

	ShaderProgram& ShaderLoader::GetShaderProgram(UUID shaderId) {
		if (shaderId == UUID::Unitialised())
			return m_DefaultShaderProgram;

		if ((shaderId == m_CachedShaderProgramId) && m_CachedShaderProgram)
			return *m_CachedShaderProgram;

		auto iterator = m_LoadedShaders.find(shaderId);

		if (iterator == m_LoadedShaders.end())
			return m_DefaultShaderProgram;

		m_CachedShaderProgramId = shaderId;
		m_CachedShaderProgram = &iterator->second;
		return iterator->second;
	}

	ShaderProgram& ShaderLoader::GetDefaultShaderProgram(DefaultShader shaderType) {
		IP_ASSERT(shaderType < DEFAULT_SHADER_COUNT, "Invalid default shader type.");
		IP_ASSERT(shaderType >= 0, "Invalid default shader type.");
		
		UUID shaderId = DefaultSahderIds[shaderType];
		IP_ASSERT(shaderId != UUID::Unitialised(), "Error getting the default shader of that type.");

		return GetShaderProgram(shaderId);
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
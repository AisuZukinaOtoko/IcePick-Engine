#pragma once
#include "../Render Systems/ShaderProgram.h"
#include "../Scene Systems/UUID.h"
#include <unordered_map>
#include <filesystem>
#include <string>

namespace IcePick {
	class ShaderLoader {
	public:
		enum DefaultShaderTypes {
			DEPTH_SHADER_PROGRAM = 0,
			NORMAL_SHADER_PROGRAM,
			PBR_SHADER_PROGRAM,
			NUM_SHADER_PROGRAMS
		};

		ShaderLoader();
		void ShutDown();
		~ShaderLoader();
		UUID LoadShaderProgram(std::filesystem::path shaderPath);
		void ReloadShaderProgram(UUID shaderId);
		ShaderProgram GetShaderProgram(UUID shaderId);
		ShaderProgram GetDefaultShaderProgram(DefaultShaderTypes shaderType);
	private:
		ShaderProgram m_DefaultShaderPrograms[NUM_SHADER_PROGRAMS];
		void RegisterShaderProgram(ShaderProgram shaderProgram);

		// Returns the shader source file with preprocessing
		std::string LoadFile(std::filesystem::path filePath, unsigned int includeDepth);
		//ShaderSource ParseShaderSourceFile();
		std::unordered_map<UUID, ShaderProgram, UUIDHasher> m_LoadedShaders;
		std::unordered_map<std::filesystem::path, UUID> m_LoadedShaderPaths;
	};
}
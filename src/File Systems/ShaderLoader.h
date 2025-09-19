#pragma once
#include "../Render Systems/ShaderProgram.h"
#include "../Scene Systems/UUID.h"
#include <unordered_map>
#include <filesystem>
#include <string>

namespace IcePick {
	class ShaderLoader {
	public:
		ShaderLoader();
		void ShutDown();
		~ShaderLoader();

		void SetDefaultShaderProgram(ShaderProgram shaderProgram);

		// Returns the shader source file with preprocessing
		std::string LoadFile(std::filesystem::path filePath, unsigned int includeDepth);
		UUID CreateShaderProgram(ShaderSource& shaderSource);

		void ReloadShaderProgram(UUID shaderId, ShaderSource& shaderSource);
		ShaderProgram& GetShaderProgram(UUID shaderId);

	private:
		ShaderProgram m_DefaultShaderProgram;
		ShaderProgram m_CachedShaderProgram;
		UUID m_CachedShaderProgramId = UUID::Unitialised();
		UUID RegisterShaderProgram(ShaderProgram shaderProgram);

		std::unordered_map<UUID, ShaderProgram, UUIDHasher> m_LoadedShaders;
		std::unordered_map<std::filesystem::path, UUID> m_LoadedShaderPaths;
	};
}
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
		void Init();
		void ShutDown();
		~ShaderLoader();

		enum DefaultShader {
			LINE_SHADER = 0,
			DEFAULT_SHADER_COUNT
		};

		void SetDefaultShaderProgram(ShaderProgram shaderProgram);

		// Returns the shader source file with preprocessing
		std::string LoadFile(std::filesystem::path filePath, unsigned int includeDepth);
		UUID CreateShaderProgram(ShaderSource& shaderSource);
		void CreateShaderProgramWithId(ShaderSource& shaderSource, UUID Id);

		void ReloadShaderProgram(UUID shaderId, ShaderSource& shaderSource);
		ShaderProgram& GetShaderProgram(UUID shaderId);
		ShaderProgram& GetDefaultShaderProgram(DefaultShader shaderType);

	private:
		ShaderProgram m_DefaultShaderProgram;
		ShaderProgram m_CachedShaderProgram;
		UUID m_CachedShaderProgramId = UUID::Unitialised();
		UUID RegisterShaderProgram(ShaderProgram shaderProgram);

		std::unordered_map<UUID, ShaderProgram, UUIDHasher> m_LoadedShaders;
		std::unordered_map<std::filesystem::path, UUID> m_LoadedShaderPaths;

		UUID DefaultSahderIds[DEFAULT_SHADER_COUNT];
	};
}
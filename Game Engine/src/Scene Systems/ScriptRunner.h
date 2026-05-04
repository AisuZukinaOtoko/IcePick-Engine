#pragma once
#include <unordered_map>
#include <filesystem>
#include <sol/sol.hpp>
#include <entt/entt.h>
#include "Components.h"
#include "Script.h"

namespace IcePick {
	class ScriptRunner {
	public:
		void Init();
		void ShutDown();
		ScriptComponent CreateScriptComponentFromFile(const std::filesystem::path& scriptPath, entt::entity entityId);

		Script& GetScriptById(UUID scriptId);
		void ReloadScripts();
		std::filesystem::path GetScriptPath(UUID scriptId);
	private:
		Script m_EmptyScript;
		UUID LoadAndRegisterScript(const std::filesystem::path& scriptPath);
		Script CreateScript(const std::string& scriptSource);
		std::string ReadScriptFile(const std::filesystem::path& scriptFilePath);

		std::unordered_map<UUID, Script, UUIDHasher> m_LoadedScripts;
		std::unordered_map<std::filesystem::path, UUID> m_LoadedScriptPaths;
		std::unordered_map<UUID, std::filesystem::path, UUIDHasher> m_LoadedScriptToScripPathMap;

		sol::state m_LuaState;
	};
} 
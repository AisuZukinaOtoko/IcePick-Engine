#pragma once
#include <unordered_map>
#include <filesystem>
#include <sol/sol.hpp>
#include <entt/entt.h>
#include "Script.h"
#include "UUID.h"

namespace IcePick {
	class PhysicsSystem3D;
	class SceneCamera;
	struct ScriptComponent;

	class ScriptRunner {
	public:
		void Init(const PhysicsSystem3D& physicsSystem, const SceneCamera& sceneCamera);
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